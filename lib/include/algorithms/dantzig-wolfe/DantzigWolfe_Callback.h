//
// Created by henri on 20/12/22.
//

class DantzigWolfe::Callback {
public:
    class Context;

    virtual void execute(Context& t_ctx) = 0;
};

class DantzigWolfe::Callback::Context {
    DantzigWolfe::Event m_event;
    DantzigWolfe& m_parent;
public:
    explicit Context(DantzigWolfe& t_parent, DantzigWolfe::Event t_event) : m_parent(t_parent), m_event(t_event) {}

    [[nodiscard]] Event event() const { return m_event; }

    const DantzigWolfe& parent() const { return m_parent; }
};