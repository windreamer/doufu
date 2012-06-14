# Build Scalable Transactional Datastore

<tianzhongbo@douban.com>

2012-06-13

---
## Tradeoffs in Datastores

* CAP Theorem
    * _Consistency_: all nodes see the same data at the same time
    * _Availability_: a guarantee that every request receives a response about whether it was successful or failed
    * _Partition tolerance_: the system continues to operate despite arbitrary message loss or failure of part of the system
* Diversity of Requirments
    * Performance
    * Scalability
    * Transaction
    * ...
---
## Comon Datastore Overview

* __Redis__, __Memcached__: high thoughput, low latency; error-prone
* __MySQL__: full ACID, rich query, decent performance; bad scalability
* __Dynamo__: linear scalability, high avalability; eventually consistency
* __MongoDB__:

---

<dot>
    digraph a { a -> b;}
</dot>
