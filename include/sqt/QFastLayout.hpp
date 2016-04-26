#ifndef __QFASTLAYOUT_HPP_1461555247__
#define __QFASTLAYOUT_HPP_1461555247__

#include <QHBoxLayout>
#include <QVBoxLayout>

namespace sqt {
    namespace FastLayout {

        class HWidget
        {
        public:
            QWidget * m_data;
            HWidget() : m_data(0) {}
            HWidget(QWidget* data) : m_data(data) {}
            ~HWidget() {}
        };

        class HLayout
        {
        public:
            QBoxLayout * m_data;
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

        HLayout operator | (HWidget lhs, HWidget rhs)
        {
            HLayout layout = new QHBoxLayout;
            layout.add(lhs);
            layout.add(rhs);
            return layout;
        }

        HLayout operator | (HLayout layout, HWidget rhs)
        {
            layout.addH(rhs);
            return layout;
        }

        HLayout operator | (HLayout layout1, HLayout layout2)
        {
            layout1.addH(layout2);
            return layout1;
        }

        HLayout operator | (HWidget lhs, HLayout layout1)
        {
            HLayout layout(new QHBoxLayout);
            layout.add(lhs);
            layout.addH(layout1);
            return layout;
        }

        HLayout operator / (HWidget lhs, HWidget rhs)
        {
            HLayout layout = new QVBoxLayout;
            layout.add(lhs);
            layout.add(rhs);
            return layout;
        }

        HLayout operator / (HLayout layout, HWidget rhs)
        {
            layout.addV(rhs);
            return layout;
        }

        HLayout operator / (HLayout layout1, HLayout layout2)
        {
            layout1.addV(layout2);
            return layout1;
        }

        HLayout operator / (HWidget lhs, HLayout layout1)
        {
            HLayout layout(new QVBoxLayout);
            layout.add(lhs);
            layout.addV(layout1);
            return layout;
        }

    } // namespace FastLayout
} // namespace sqt




#endif /* __QFASTLAYOUT_HPP_1461555247__ */
