#include "Input.h"
namespace NBE
{
	std::shared_ptr<Input> Input::Inputs[MAX_INPUTS];
	size_t Input::totalInputNum = 0;
}