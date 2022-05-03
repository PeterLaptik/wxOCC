#include "../include/objectpool.h"

ObjectPool::ObjectPool()
{ }

ObjectPool::~ObjectPool()
{
    m_shapes.clear();
}


void ObjectPool::AppendObject(Handle(AIS_InteractiveObject) shape)
{
    m_shapes.push_back(shape);
}

bool ObjectPool::Contains(Handle(AIS_InteractiveObject) object) const
{
    for(auto shape: m_shapes)
        if(object==shape)
            return true;
    return false;
}
