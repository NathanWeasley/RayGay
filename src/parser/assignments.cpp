
#include "parser/assignments.h"
#include "exception.h"
#include "runtimeexception.h"

Assignments* Assignments::getUniqueInstance() {
    static Assignments unique_instance;
    return &unique_instance;
}

Assignments::Assignments() {

}

Path* Assignments::getNamedPath(string name, FilePosition pos) {
    Path* result = pathMap[name];
    if (result == NULL) {
	throw RuntimeException("Path named $"+name+" not defined",pos);
    } 
    return result;
}

void Assignments::setNamedPath(string name, Path* path) {
    pathMap[name] = path;
}

double Assignments::getNamedFloat(string name, FilePosition pos) {
    // TODO: Throw exception
    return floatMap[name];
}

void Assignments::setNamedFloat(string name, double val) {
    floatMap[name] = val;
}

Vector Assignments::getNamedVector(string name, FilePosition pos) {
    // TODO: Throw exception
    return vectorMap[name];
}

void Assignments::setNamedVector(string name, Vector v) {
    vectorMap[name] = v;
}

SceneObject* Assignments::getNamedSceneObject(string name, FilePosition pos) {
    SceneObject* result = objectMap[name];
    if (result == NULL) {
	throw RuntimeException("SceneObject named $"+name+" not defined",pos);
    }
    return result;
}

void Assignments::setNamedSceneObject(string name, SceneObject* obj) {
    objectMap[name] = obj;
}

Material* Assignments::getNamedMaterial(string name, FilePosition pos) {
    Material* result = materialMap[name];
    if (result == NULL) {
	throw RuntimeException("Material named "+name+" not defined",pos);
    }
    return result;
}

void Assignments::setNamedMaterial(string name, Material* material) {
    materialMap[name] = material;
}

Function* Assignments::getNamedFunction(string name, FilePosition pos) {
    Function* result = functionMap[name];
    if (result == NULL) {
	throw RuntimeException("Function named "+name+" not defined",pos);
    }
    return result;

}

void Assignments::setNamedFunction(string name, Function* function) {
    functionMap[name] = function;
}
