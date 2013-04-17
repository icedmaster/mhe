#include "platform/ios/ios_log.hpp"
#import <Foundation/Foundation.h>

namespace mhe {
namespace utils {
namespace ios {

void iOSLog::write_impl(const std::string& str)
{
	NSLog(@"%@", [NSString stringWithUTF8String:str.c_str()]);
}

}}}
