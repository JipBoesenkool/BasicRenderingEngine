//
// Created by Jip Boesenkool on 13/08/2017.
// https://designingefficientsoftware.wordpress.com/2014/10/01/generic-class-factory-introduction-part-14/
//
#ifndef PINE_GENERICOBJECTFACTORY_H
#define PINE_GENERICOBJECTFACTORY_H
//---------------------------------------------------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------------------------------------------------
#include <map>
template <class TKey>
class GenericObjectFactory
{
	// This class implements a generic factory that can be used to create any type.  Template arguments:
	// TKey - the first class template type is provided so that the object type name can be defined as a string, int, double, etc.
	// To use this class properly, the static CreateObj methods in the concrete classes to be created must be defined to return a
	// pointer to the created object as void *.  This means that any type can be stored in the hash table.
	//
	// The register method must be called to map the object type name (the TKey template argument) to the method that creates the object.  Example:
	// GenericFactory<string, Animal> GenFactAnimal; // Creates a class factory that can create derived objects of the Animal base class.
	// GenFactAnmal.Register("dog", &Dog::CreateObject); // Maps the string "dog" to the Dog CreateObject method, which must be declared as static.
	//
	// The CreateObj method is called to create an object.  The object type name must be the same as the one registered.  The return value is always void*.
	// This means that this pointer will have to be cast to the proper type.  Example with a unique_ptr:
	// unique_ptr<Color> pRedColor((static_cast<Color*>(GenFact.CreateObj("red"))));
	//
	typedef void* (*CreateObjFn)(std::string &args); // Defines the CreateObjFn function pointer that points to the object creation function.
	typedef std::map<TKey, CreateObjFn> FactoryMap; // Hash table to map the key to the function used to create the object.
public:
	void Register(const TKey &keyName, CreateObjFn pCreateFn)
	{
		// Store the object type name in the hash table.
		FactMap[keyName] = pCreateFn;
	}

	/*
	template <class TType>
	TType* Create(const TKey &keyName, std::vector<void*> &args)
	{
		// This method looks for the name in the hash map.  If it is not found, then null is returned.
		// If it is found, then it creates the specified object returns a pointer to it.
		// Object parameters are vector of typeless parameter
		FactoryMap::iterator It = FactMap.find(keyName);
		if (It != FactMap.end())
		{
			void* pObj = It->second(args); // Call the method to create the object.
			TType* Obj = static_cast<TType*>(pObj); // Cast the object to the specified type.
			return Obj;
		}
		throw "GenericObjectFactory::Create: key was not found in table.  Did you forget to register it?";
	}
	 */

	template <class TType>
	TType* Create(const TKey &keyName, std::string args = "")
	{
		// This method looks for the name in the hash map.  If it is not found, then null is returned.
		// If it is found, then it creates the specified object and returns a pointer to it.
		// object parameters are in json format
		typename FactoryMap::iterator It = FactMap.find(keyName);
		if (It != FactMap.end())
		{
			void* pObj;
			if( args.length() == 0 )
			{
				pObj = It->second(); // Call the method to create the object.
			}
			else
			{
				pObj = It->second(args); // Call the method to create the object.
			}
			TType* Obj = static_cast<TType*>(pObj); // Cast the object to the specified type.
			return Obj;
		}
		throw "GenericObjectFactory::Create: key was not found in table.  Did you forget to register it?";
	}
private:
	FactoryMap FactMap;
};

#endif //PINE_GENERICOBJECTFACTORY_H
