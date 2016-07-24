/**
 *  Copyleft(c) 549506937@qq.com
 *  author:sarrow
 *
 *  @file  sqt/QFastLayout.hpp
 *
 *  @brief FastLayout for qt widget
 */
#ifndef __QFASTLAYOUT_HPP_1461555247__
#define __QFASTLAYOUT_HPP_1461555247__

#include <QBoxLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>

#include <QWidget>
#include <QDebug>

#include <vector>
#include <deque>
#include <initializer_list>

#ifndef VALUE_MSG
#define VALUE_MSG(a) (#a) << " = " << (a) << ";"
#endif

namespace sqt {
    namespace FastLayout {

        class HWidget
        {
        public:
            QWidget * m_data;
            int m_row_span;
            int m_col_span;
            HWidget() : m_data(0), m_row_span(1), m_col_span(1) {}
            HWidget(QWidget* data) : m_data(data), m_row_span(1), m_col_span(1) {}
            HWidget(QWidget* data, int row_span, int col_span) : m_data(data), m_row_span(std::max(1, row_span)), m_col_span(std::max(1, col_span))
            {}
            HWidget(int row_span, int col_span) : m_data(0), m_row_span(std::max(1, row_span)), m_col_span(std::max(1, col_span)) {
                qDebug() << this << m_row_span << m_col_span;
            }
            ~HWidget() {}
            QWidget * parentWidget() {
                return m_data ? m_data->parentWidget() : 0;
            }
            operator bool () const {
                return m_data;
            }
            bool is_multy_cell() const {
                return (m_row_span > 1 || m_col_span > 1);
            }
        };

        // NOTE QWidget 只能关联一次layout！
        // 如果一定要reset layout；需要：
        // delete myWidget->layout();
        class HLayout
        {
        public:
            QBoxLayout * m_data;
            explicit HLayout(Qt::Orientations o) : m_data(0) {
                if (o == Qt::Horizontal) {
                    m_data = new QHBoxLayout;
                }
                else {
                    m_data = new QVBoxLayout;
                }
            }
            HLayout(QBoxLayout* data) : m_data(data) {}
            ~HLayout() {}

            void add(const HWidget & h) {
                if (h.m_data) {
                    m_data->addWidget(h.m_data);
                }
                else {
                    m_data->addStretch();
                }
            }
            void ensureV() {
                QVBoxLayout * v_layout = dynamic_cast<QVBoxLayout*>(m_data);
                if (!v_layout) {
                    v_layout = new QVBoxLayout;
                    v_layout->addLayout(m_data);
                    m_data = v_layout;
                }
            }
            void ensureH() {
                QHBoxLayout * h_layout = dynamic_cast<QHBoxLayout*>(m_data);
                if (!h_layout) {
                    h_layout = new QHBoxLayout;
                    h_layout->addLayout(m_data);
                    m_data = h_layout;
                }
            }
            void addV(const HWidget & h) {
                this->ensureV();
                this->add(h);
            }

            void addH(const HWidget & h) {
                this->ensureH();
                this->add(h);
            }

            void addV(const HLayout & h) {
                this->ensureV();
                this->m_data->addLayout(h.m_data);
            }

            void addH(const HLayout & h) {
                this->ensureH();
                this->m_data->addLayout(h.m_data);
            }
            operator QBoxLayout * () const {
                return this->m_data;
            }
        };

        class HBoxLayout {
        private:
            union data_t {
                QBoxLayout * layout;
                QWidget *   widget;
            } m_value;
            enum type_t {
                null_type = 0,
                widget_type,
                layout_type
            } m_type;
        public:
            HBoxLayout()
            {
                this->init();
            }
            HBoxLayout(QBoxLayout * l)
            {
                this->init(l);
            }
            HBoxLayout(QWidget * w)
            {
                this->init(w);
            }

        protected:
            void init()
            {
                this->m_value.layout = 0;
                this->m_type = null_type;
            }
            void init(QWidget * w)
            {
                this->m_value.widget = w;
                this->m_type = widget_type;
            }
            void init(QBoxLayout * l)
            {
                this->m_value.layout = l;
                this->m_type = layout_type;
            }
            QBoxLayout * getLayout() const
            {
                return this->m_value.layout;
            }
            QWidget * getWidget() const
            {
                return this->m_value.widget;
            }

        public:
            HBoxLayout(std::initializer_list<HBoxLayout> iw, Qt::Orientation orientation = Qt::Horizontal)
            {
                this->init(new QBoxLayout(orientation == Qt::Horizontal ? QBoxLayout::LeftToRight : QBoxLayout::TopToBottom));
                for (auto & i : iw) {
                    switch (i.m_type) {
                    case null_type:
                        getLayout()->addStretch();
                        break;

                    case widget_type:
                        getLayout()->addWidget(i.getWidget());
                        break;

                    case layout_type:
                        getLayout()->addLayout(i.getLayout());
                        break;
                    }
                }
            }
            static HBoxLayout horizontal(std::initializer_list<HBoxLayout> iw)
            {
                return HBoxLayout(iw, Qt::Horizontal);
            }
            static HBoxLayout vertical(std::initializer_list<HBoxLayout> iw)
            {
                return HBoxLayout(iw, Qt::Vertical);
            }
            operator QBoxLayout * () const {
                return this->m_type == layout_type ? m_value.layout : 0;
            }
            operator bool () const {
                return this->m_type != null_type;
            }
        };

        class HGridLayout {
            QGridLayout * m_data;
        public:
            // NOTE 此时，退化为QHBoxLayout
            // 基于QGridLayout的布局，表达能力有限；
            // 当需要更加自由的时候，还是需要QHBoxLayout和QVBoxLayout嵌套组合的方式；
            // 此时，如何利用初始化列表呢？
            // 最开始，HLayout，我是利用操作符重载，来避开手动创建QLayout对象的
            // 问题——采用的运算符，决定了两个对象所生成的QLayout对象的布局方
            // 向。
            // 如果要利用初始化列表，我也必须提供"方向"信息；不然，仅靠{}的循环
            // 嵌套，是不容易决定方向的，不管是编译器，还是对本库的使用者。
            // 初始化列表应用情况：
            // 1. 构造函数；形如：{T1 a, T2 b, ... }此时，允许不同类型；但是，实参数目相对固定；
            // 2. 使用std::initializer_list列表；此时要求每个元素可以创建成相同的类型；
            //    如果，要求嵌套，那么相当于内部的{}表达式，也能被构造为相同类型的对象！
            // 这就是说，如果我要嵌套，要么就要求构造参数，也能接受本类型临时对象，作为参数！
            // 当然，需要注意的是，std::initializer_list就是一种普通的参数类型
            // ，不是构造函数专用，返回对象的static成员函数，也能使用！
            //
            // 参考：
            //
            //! file://home/sarrow/Sources/json/src/json.hpp|1617
            // 为了能让C++编译器，解析
            // json j2 = {
            //   {"pi", 3.141},
            //   {"happy", true},
            //   {"name", "Niels"},
            //   {"nothing", nullptr},
            //   {"answer", {
            //     {"everything", 42}
            //   }},
            //   {"list", {1, 0, 2}},
            //   {"object", {
            //     {"currency", "USD"},
            //     {"value", 42.99}
            //   }}
            // };
            //
            // 这样的串，并且正确区分json_array和json_object；它上述的构造函数
            // ，对std::initializer_list所获取到的内容做了判断；
            // 比如：
            //  {"answer", {
            //    {"everything", 42}
            //  }},
            //
            //  中间的 {"everything", 42} 可以被理解为 json的key-value对，也可以被理解为 json的仅有两个元素的array；
            //
            //  为此，提供了面向用户的static函数：
            //
            // static basic_json array(std::initializer_list<basic_json> init =
            //                             std::initializer_list<basic_json>())
            // {
            //     return basic_json(init, false, value_t::array);
            // }
            //
            // basic_json::basic_json(std::initializer_list<basic_json> init,
            //                        bool type_deduction = true,
            //                        value_t manual_type = value_t::array);
            // 简言之，如果不限定层次(就是递归{})，我需要让
            // std::initializer_list，也能接受本类型对象的参数类型；

            // {
            //  {{b1}, {b2}},
            //  {{b3, 1, 2}},
            //  {{b4}, {b5}}
            // }
            HGridLayout(std::initializer_list<std::initializer_list<HWidget> > il)
                : m_data(new QGridLayout)
            {
                std::deque<std::vector<int> > used_cell;
                int r = -1;
                int c = -1;
                for (auto & row : il) {
                    r ++;
                    c = 0;
                    for (auto & item : row) {
                        // 找空位！
                        if (!used_cell.empty()) {
                            while (std::find(used_cell[0].cbegin(), used_cell[0].cend(), c) != used_cell[0].cend()) {
                                ++c;
                            }
                        }
                        if (item.is_multy_cell()) {
                            if (item.m_data) {
                                m_data->addWidget(item.m_data, r, c,
                                                  item.m_row_span, item.m_col_span);
                            }
                            // 添加一个
                            // 0-col
                            // ...
                            // row
                            // 形状的矩形区域；同时，排除第一行；因为，第一行，只需要考虑m_col_span即可；
                            for (int i = 1; i < item.m_row_span; ++i) {
                                while (int(used_cell.size()) < i + 1) {
                                    used_cell.push_back(std::vector<int>());
                                }
                                for (int j = 0; j < item.m_col_span; ++j) {
                                    used_cell[i].push_back(c + j);
                                }
                            }
                        }
                        else {
                            if (item.m_data) {
                                m_data->addWidget(item.m_data, r, c);
                            }
                        }
                        c += item.m_col_span;
                    }
                    if (!used_cell.empty()) {
                        used_cell.pop_front();
                    }
                }
            }

        public:
            operator QLayout *() {
                return m_data;
            }
        };

        // NOTE 可以创建，基于 initializer_list 初始化列表的 HBoxLayout 吗？
        // HBoxLayout要达到的目的是，用{}嵌套风格，来创建复合 QHBoxLayout, QVBoxLayout的ui界面。
        //
        // 问题是 initializer_list 的层数，是在定义的时候，就确定了的；
        //
        // 比如，上面的 HGridLayout 必须是三层{}嵌套；
        // 而我这个 HBoxLayout，理论上是不限制嵌套层数的……
        //! https://github.com/nlohmann/json
        //
        // 当然，如果觉得不控制嵌套层数的话，可以用类似：
        // json array_not_object = { json::array({"currency", "USD"}), json::array({"value", 42.99}) };
        //
        // 对比，同样效果的：
        //
        // j["object"] = { {"currency", "USD"}, {"value", 42.99} };
        // nlohmann/json 通过，构造 多个 basic_json构造函数（当然，还有
        // std::initializer_list<basic_json> 参数的版本），来完成，递归构造；
        // 不过，由于C++的initializer_list，只支持{}的嵌套——原本jsson中是{}、[]分开，以区分object和array；
        // 于是C++11版的，json解释器，不能完全依赖于initializer_list构造——有二义性；
        //   {{"key",1.2}}
        // 上面这个初始化列表，可能被理解为一个object，但也可能被理解为两重数组！
        //
        // 于是 nlohmann/json，针对initializer_list版构造函数，还额外提供了用户手动(显式)确认的参数。
        //
        // 见：
        //
        // $nlohmann/json/src/json.hpp|1640

        inline HLayout operator | (HWidget lhs, HWidget rhs)
        {
            HLayout layout(Qt::Horizontal);
            layout.add(lhs);
            layout.add(rhs);
            return layout;
        }

        inline HLayout operator | (HLayout layout, HWidget rhs)
        {
            layout.addH(rhs);
            return layout;
        }

        inline HLayout operator | (HLayout layout1, HLayout layout2)
        {
            layout1.addH(layout2);
            return layout1;
        }

        inline HLayout operator | (HWidget lhs, HLayout layout1)
        {
            HLayout layout(Qt::Horizontal);
            layout.add(lhs);
            layout.addH(layout1);
            return layout;
        }

        inline HLayout operator / (HWidget lhs, HWidget rhs)
        {
            HLayout layout(Qt::Vertical);
            layout.add(lhs);
            layout.add(rhs);
            return layout;
        }

        inline HLayout operator / (HLayout layout, HWidget rhs)
        {
            layout.addV(rhs);
            return layout;
        }

        inline HLayout operator / (HLayout layout1, HLayout layout2)
        {
            layout1.addV(layout2);
            return layout1;
        }

        inline HLayout operator / (HWidget lhs, HLayout layout1)
        {
            HLayout layout(Qt::Vertical);
            layout.add(lhs);
            layout.addV(layout1);
            return layout;
        }

    } // namespace FastLayout
} // namespace sqt




#endif /* __QFASTLAYOUT_HPP_1461555247__ */
