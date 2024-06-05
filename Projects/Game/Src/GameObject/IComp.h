#pragma once
#include "Camera/Camera.h"

class Object;

class IComp {
public:
	IComp(Object* const object):
		object_(*object)
	{}
	virtual ~IComp() = default;
	IComp() = delete;
	IComp(const IComp&) = delete;
	IComp(IComp&&) = delete;

	IComp& operator=(const IComp&) = delete;
	IComp& operator=(IComp&&) = delete;

public:
	virtual void AddComp(){}
	virtual void Init(){}
	virtual void Finalize(){}

	virtual void Update0(){}
	virtual void Update1(){}
	virtual void Update2(){}
	virtual void Update3(){}
	virtual void Update4(){}

	virtual void Draw(const Camera&) {}

protected:
	Object& object_;
};