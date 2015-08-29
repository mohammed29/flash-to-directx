# Introduction #

There is some known issues with AS2-based Flash movies that uses keyboard (games).


# Details #

For some unknown yet reason, AS2 function Key.isDown() returns no press in case focus is on application that uses library. In-depth review shows that Flash calls GetKeyState() while processing WM\_TIMER event (is it valid at all? why not GetAsyncKeyState()?). If host application removes focus from it's window while forwarding WM\_TIMER through TranslateMessage()/DispatchMessage() functions, Key.isDown() works as expected.

We've considered this as not an issue for GUI only side of DirectX application and stopped further research on this topic.