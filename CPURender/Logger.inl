template<typename ...Args>
inline void Logger::Log(const Args & ...args)
{
    if (!s_canLog)
        return;
    (std::cout << ... << args) << '\n';
}
