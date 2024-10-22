#include "val.h"

// numeric overloaded multiply this by op
Value Value::operator*(const Value& op) const {
    if (this->IsInt() && op.IsInt()) {
        return Value(this->GetInt() * op.GetInt());
    } else if (this->IsReal() && op.IsReal()) {
        return Value(this->GetReal() * op.GetReal());
    } else {
        return Value(); // return error value
    }
}

// overloaded less than operator of this with op
Value Value::operator<(const Value& op) const {
    if (this->IsInt() && op.IsInt()) {
        return Value(this->GetInt() < op.GetInt());
    } else if (this->IsReal() && op.IsReal()) {
        return Value(this->GetReal() < op.GetReal());
    } else {
        return Value(); // return error value
    }
}

// string concatenation of this with op
Value Value::Catenate(const Value & op) const {
    if (this->IsString() && op.IsString()) {
        return Value(this->GetString() + op.GetString());
    } else {
        return Value(); // return error value
    }
}

// compute the value of this raised to the exponent op
Value Value::Power(const Value & op) const {
    if (this->IsReal() && op.IsReal()) {
        return Value(pow(this->GetReal(), op.GetReal()));
    } else {
        return Value(); // return error value
    }
}
