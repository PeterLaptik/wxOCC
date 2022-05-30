#ifndef OBJECTPOOL_H_INCLUDED
#define OBJECTPOOL_H_INCLUDED

#include <AIS_Shape.hxx>

///\brief Geometric object handler.
class ObjectPool
{
    public:
        ObjectPool();
        ~ObjectPool();
        void AppendObject(Handle(AIS_InteractiveObject) object);
        bool Contains(AIS_InteractiveObject *object) const;
        Handle(AIS_InteractiveObject) GetLast(void) const
        {
            return m_objects[m_objects.size()-1];
        }

    private:
        std::vector<Handle(AIS_InteractiveObject)> m_objects;

};

#endif // OBJECTPOOL_H_INCLUDED
