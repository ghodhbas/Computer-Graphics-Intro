#ifndef GROUP_H
#define GROUP_H

#include "Object3D.h"
#include "Ray.h"
#include "Hit.h"
#include <iostream>

using  namespace std;

///TODO:
///Implement Group
///Add data structure to store a list of Object*
class Group :public Object3D
{
public:

	Group() {
	}

	Group(int num_objects) {
		nb_objects = num_objects;
		group = vector<Object3D*>(nb_objects);
	}

	~Group() {
	}

	virtual bool intersect(const Ray& r, Hit& h, float tmin) {
		bool result = false;
		for (int i = 0; i < nb_objects; i++)
		{
			if (group[i]->intersect(r, h, tmin)) result = true;
		}
		return result;
	}

	void addObject(int index, Object3D* obj) {
		group[index] = obj;
	}

	int getGroupSize() {
		return nb_objects;
	}

private:
	int nb_objects;
	vector<Object3D*> group;
};

#endif
