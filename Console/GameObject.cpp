#include "GameObject.h"
#include "DDS.h"
#include "common_enum.h"

void GameObject::set_type(Type type) {
    this->type = type;
}
void GameObject::set_move(int dx, int dy) {
    move_dx = dx;
    move_dy = dy;
}
std::pair<int, int> GameObject::get_move() {
    return { move_dx,move_dy };
}
GameObject::GameObject() :GameObject(UNKNOW, 0, 0) {} // c++11 委托构造函数, 并且使用条款04：确定对象使用前已先被初始化
GameObject::GameObject(Type type) :GameObject(type, 0, 0) {}
GameObject::GameObject(Type type, int move_dx, int move_dy) :type(type), move_dx(0), move_dy(0) {}
GameObject::Type GameObject::getType()const { return type; }
IMG_TYPE GameObject::getImgType() const
{
    IMG_TYPE img_type = IMG_TYPE::IMG_BLANK;
    switch (this->type)
    {
    case BOX:
        img_type = IMG_TYPE::IMG_BOX;
        break;
    case PLAYER:
        img_type = IMG_TYPE::IMG_PLAYER;
        break;
    case TARGET:
        img_type = IMG_TYPE::IMG_TARGET;
        break;
    case BOUNDARY:
        img_type = IMG_TYPE::IMG_BOUNDARY;
        break;
    case BLANK:
        img_type = IMG_TYPE::IMG_BLANK;
        break;
    case BOX_READY:
        img_type = IMG_TYPE::IMG_BOX_READY;
        break;
    case PLAYER_HIT:
        img_type = IMG_TYPE::IMG_PLAYER_HIT;
        break;
    default:
        break;
    }
    return img_type;
}
bool GameObject::operator==(const GameObject& other)const {
    return this->type == other.getType() && this->move_dx == other.move_dx && this->move_dy == other.move_dy;
}
bool GameObject::operator!=(const GameObject& other)const {
    return !(*this == other);
}
bool GameObject::operator==(Type type)const {
    return this->type == type;
}
bool GameObject::operator!=(Type type)const {
    return !(*this == type);
}
GameObject& GameObject::operator=(Type type) {
    this->type = type;
    return *this;
}
GameObject& GameObject::operator=(char c)
{
    this->type = static_cast<Type> (c);
    return *this;
}
GameObject::operator char()const {
    return (char)this->type;
}
std::ostream& operator<<(std::ostream& out, const GameObject& go) {
    return out << (char)go.getType();
}