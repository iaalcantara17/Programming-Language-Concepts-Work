#include "val.h"
#include <cmath>

Value Value::operator<(const Value& operand) const 
{
    if (this->IsInteger() && operand.IsInteger()) 
    {
        return Value(this->GetInteger() < operand.GetInteger());
    }
    else if ((this->IsInteger() && operand.IsDouble()) || (this->IsDouble() && operand.IsInteger()) || (this->IsDouble() && operand.IsDouble())) 
    {
        return Value(this->GetDouble() < operand.GetDouble());
    }
    else 
    {
        return Value();
    }
}

Value Value::Catenate(const Value & operand) const 
{
    if (this->IsText() && operand.IsText()) 
    {
        return Value(this->GetText() + operand.GetText());
    }
    else 
    {
        return Value();
    }
}

Value Value::operator*(const Value& operand) const 
{
    if (this->IsInteger() && operand.IsInteger()) 
    {
        return Value(this->GetInteger() * operand.GetInteger());
    }
    else if ((this->IsInteger() && operand.IsDouble()) || (this->IsDouble() && operand.IsInteger()) || (this->IsDouble() && operand.IsDouble())) 
    {
        return Value(this->GetDouble() * operand.GetDouble());
    }
    else 
    {
        return Value();
    }
}

Value Value::Power(const Value & operand) const 
{
    if ((this->IsInteger() && operand.IsDouble()) || (this->IsDouble() && operand.IsInteger()) || (this->IsDouble() && operand.IsDouble())) 
    {
        return Value(pow(this->GetDouble(), operand.GetDouble()));
    }
    else 
    {
        return Value();
    }
}
