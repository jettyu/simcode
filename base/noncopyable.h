#ifndef SN_NONCOPYABLE_H
#define SN_NONCOPYABLE_H
namespace simcode
{

class noncopyable
{
public:

protected:
    /** Default constructor */
    noncopyable() {}
    /** Default destructor */
    virtual ~noncopyable() {}

private:
    noncopyable( const noncopyable& ) {}
    const noncopyable& operator=( const noncopyable& ) {return *this;}
};
}//end of namespace sim

#endif // SN_NONCOPYABLE_H
