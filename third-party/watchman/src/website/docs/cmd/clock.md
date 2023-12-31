---
title: clock
category: Commands
---

Returns the current clock value for a watched root.

_Since 3.9._

_The [capability](capabilities.md) name associated with this enhanced
functionality is `clock-sync-timeout`._

`sync_timeout` specifies the number of milliseconds that you want to wait to
observe a synchronization cookie. The synchronization cookie is created at the
start of your clock call and, once the cookie is observed, means that the clock
value returned by this command is at least as current as the time of your clock
call.

If no `sync_timeout` is specified, the returned clock value is the instantaneous
value of the clock associated with the watched root, and may be almost
immediately invalidated if there are any filesystem notifications that are yet
to be processed.

```bash
$ watchman clock /path/to/dir
```

JSON:

Note the third options argument is optional.

```json
["clock", "/path/to/dir", {"sync_timeout": 100}]
```
