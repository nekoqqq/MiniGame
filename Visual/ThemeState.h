class RootState;
struct DDS;

class ThemeState { // 主题状态
public:
    ThemeState();
    ~ThemeState();
    void update(RootState* parent);
private:
    DDS* theme_img;
    int selection;
};