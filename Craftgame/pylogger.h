#pragma once


void pylogger_info(std::string s)
{
	CGE::instance->logger->info(s);
}
void pylogger_warn(std::string s)
{
	CGE::instance->logger->warn(s);
}
void pylogger_err(std::string s)
{
	CGE::instance->logger->err(s);
}
BOOST_PYTHON_MODULE(logger)
{
	boost::python::def("info", pylogger_info);
	boost::python::def("warn", pylogger_warn);
	boost::python::def("err", pylogger_err);
}
