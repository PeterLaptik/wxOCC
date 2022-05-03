#include "../include/objectpool.h"

ObjectPool::ObjectPool()
{ }

ObjectPool::~ObjectPool()
{
    m_objects.clear();
}


void ObjectPool::AppendObject(Handle(AIS_InteractiveObject) object)
{
    m_objects.push_back(object);
}

bool ObjectPool::Contains(AIS_InteractiveObject *object) const
{
    if(object==nullptr)
        return false;

    for(auto obj_handler: m_objects)
        if(object==obj_handler.get())
            return true;

    return false;
}
