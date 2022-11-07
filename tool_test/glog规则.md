# glog规则

> 注意：DLOG_ASSERT 有bug，拷贝目录install下的文件进行安装

Debug下：

- 使用DLOG、DVLOG和DLOG_ASSERT
- DLOG_IF, DLOG_EVERY_N, DLOG_IF_EVERY_N, DLOG_EVERY_T
- DCHECK，DCHECK_NOTNULL，DCHECK_STREQ，DCHECK_STRCASEEQ

非debug

- LOG, VLOG
- LOG_IF
- CHECK
- PLOG

## 使用

非DEBUG的使用情况：

- 环境初始化，关键信息
- 非性能关键路径
- 性能关键路径实用Debug模式的log，以便可以去除

DEBUG的使用情况

- DLOG: FATAL、ERROR、WARNING、INFO，其中尽量避免使用FATAL
- DVLOG 只使用三级，0线程或者过程级别、1模块信息、2函数级信息、3详细的debug信息

养成写log的习惯，方便调试。
