#ifndef __PROFILER_HPP__
#define __PROFILER_HPP__

#include <map>
#include <vector>
#include "core/hash.hpp"
#include "core/singleton.hpp"
#include "core/string.hpp"
#include "utils/sysutils.hpp"

namespace mhe {

class Profiler;
class ProfilerElement;

typedef uint32_t ProfilerPointId;
const ProfilerPointId empty_profiler_point = static_cast<ProfilerPointId>(-1);

class Profiler
{
public:
    struct Data
    {
        string name;
        string data;
        size_t count;
        float interval;
        ProfilerPointId id;
        ProfilerPointId parent_id;

        Data() : count(0), interval(0.0f), parent_id(empty_profiler_point) {}
    };
public:
    Profiler() : current_point_(empty_profiler_point) {}

    void start(ProfilerElement* element);
    void finish(ProfilerElement* element);

    void clear();

    std::vector<Profiler::Data> data() const;
private:
    void prepare_output_data(std::vector<Profiler::Data>& data) const;

    std::vector<Profiler::Data> data_;
#ifdef PROFILER_SAVE_LAST_FRAME_DATA
    std::vector<Profiler::Data> last_frame_data_;
#endif
    ProfilerPointId current_point_;
};

class MainProfiler : public Profiler, public Singleton<MainProfiler>
{
    friend class Singleton<MainProfiler>;
private:
    MainProfiler() {}
    ~MainProfiler() {}
};

class MHE_EXPORT ProfilerElement
{
public:
    enum
    {
        single,
        add
    };
public:
    ProfilerElement(const char* name = nullptr, int mode = single, Profiler& profiler = MainProfiler::instance(), const char* data = 0);
    ~ProfilerElement();

    void stop();

    const char* name() const
    {
        return name_;
    }

    float result() const
    {
        return result_;
    }

    int mode() const
    {
        return mode_;
    }

    const char* data() const
    {
        return data_;
    }
private:
    utils::Timer timer_;
    float result_;
    const char* name_;
    Profiler& profiler_;
    int mode_;
    const char* data_;
};

#define PROFILE(name) ProfilerElement pe##__LINE__(name)

}

#endif
