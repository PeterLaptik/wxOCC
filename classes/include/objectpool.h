#ifndef OBJECTPOOL_H_INCLUDED
#define OBJECTPOOL_H_INCLUDED

#include <AIS_Shape.hxx>

class ObjectPool
{
    public:
        ObjectPool();
        ~ObjectPool();
        void AppendObject(Handle(AIS_InteractiveObject) object);
        bool Contains(AIS_InteractiveObject *object) const;

    private:
        std::vector<Handle(AIS_InteractiveObject)> m_objects;

};

#endif // OBJECTPOOL_H_INCLUDED
