# Introduction #

Few specific details to make calls between AS3 and C++.

# Details #

To make a call of C++ function from AS3 you need to make the following:
  * In C++ register function with _ASInterface::AddCallback(name\_for\_AS3, class, function)_;
  * Call function from AS3: _ExternalInterface.call(name\_for\_AS3, parameters)_;

To make a call of AS3 function from C++ you need to make the following:
  * Register AS3 function for external interface: _ExternalInterface.addCallback(name\_for\_C++, AS3\_function)_;
  * Call function from C++: _ASInterface::Call(name\_for\_C++, parameters)_;

You can use any number of parameters for AS3 and C++ calls.