//
// Created by henri on 23/11/22.
//
#include "../../../include/modeling/objects/ObjectId.h"

unsigned int ObjectId::s_id = 0;

ObjectId::ObjectId(unsigned int t_index, std::string &&t_name, std::string &&t_default_name)
    : m_id(s_id++),
      m_index(t_index),
      m_name(!t_name.empty() ? std::move(t_name) : std::move(t_default_name) + "__" + std::to_string(m_id))
    {

}

