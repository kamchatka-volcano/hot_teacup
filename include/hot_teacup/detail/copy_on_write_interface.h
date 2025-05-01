#ifndef HOT_TEACUP_COPY_ON_WRITE_INTERFACE_H
#define HOT_TEACUP_COPY_ON_WRITE_INTERFACE_H

namespace http::detail {

class ICopyOnWrite {
public:
    virtual ~ICopyOnWrite() = default;
    virtual bool isView() const = 0;
    virtual void makeOwnStateFromView() = 0;
};

}

#endif //HOT_TEACUP_COPY_ON_WRITE_INTERFACE_H