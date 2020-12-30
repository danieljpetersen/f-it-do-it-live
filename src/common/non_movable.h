#ifndef FI_NON_MOVE
#define FI_NON_MOVE

class Non_Movable
{
    public:
        Non_Movable(Non_Movable&&) = delete;

        Non_Movable& operator=(Non_Movable&&) = delete;

    protected:
        Non_Movable() = default;
};

#endif 
