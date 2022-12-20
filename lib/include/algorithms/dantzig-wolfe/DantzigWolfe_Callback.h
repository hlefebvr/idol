//
// Created by henri on 20/12/22.
//

template<class ParentT>
class UserCallback : public AbstractCallback {
    ParentT* m_parent = nullptr;
    friend class Algorithm;
public:
    ParentT &parent() final { return *m_parent; }

    [[nodiscard]] const ParentT &parent() const final { return *m_parent; }

    using type = ParentT;
};
