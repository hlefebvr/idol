//
// Created by henri on 01/09/22.
//

#ifndef OPTIMIZE_ENV_H
#define OPTIMIZE_ENV_H

#include <list>
#include <memory>
#include "../Types.h"

namespace impl {
    class Object;
}

template<enum Player> class BaseModel;

class Env;

class ObjectCreator {
protected:
    friend class BaseModel<Decision>;
    friend class BaseModel<Parameter>;

    template<class T, class ...Args>
    T* create(Args ...t_args) {
        auto* ptr_to_object = new T(env(), std::forward<Args>(t_args)...);
        save_object(ptr_to_object);
        return ptr_to_object;
    }

    virtual void save_object(impl::Object* t_ptr_to_object) = 0;

    virtual Env& env() = 0;

    [[nodiscard]] virtual const Env& env() const = 0;
};

class ObjectDestructor {
protected:
    friend class impl::Object;
    virtual void free(const std::list<impl::Object*>::iterator& t_it) = 0;
};

class Env : public ObjectCreator, public ObjectDestructor {
    std::list<impl::Object*> m_objects;

    void save_object(impl::Object *t_ptr_to_object) override;
    void free(const std::list<impl::Object*>::iterator& t_it) override;
    inline Env &env() override;
    [[nodiscard]] inline const Env &env() const override;
public:
    Env() = default;

    Env(const Env&) = delete;
    Env(Env&&) noexcept = delete;

    Env& operator=(const Env&) = delete;
    Env& operator=(Env&&) noexcept = delete;

    ~Env();
};


Env &Env::env() {
    return *this;
}

const Env &Env::env() const {
    return *this;
}

#endif //OPTIMIZE_ENV_H
