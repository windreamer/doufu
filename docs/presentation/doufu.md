# Build Scalable Transactional Datastore

<tianzhongbo@douban.com>  
2012-06-13

---
## Datastore Restrictions

* CAP Theorem
    * Consistency: 
        * all nodes see the same data at the same time
    * Availability: 
        * a guarantee that every request receives a response about whether it was successful or failed
    * Partition tolerance: 
        * the system continues to operate despite arbitrary message loss or failure of part of the system
* Trade-offs 
    * Performance
    * Scalability
    * Transactions
    * ...

---
## Datastore Overview

* Redis, Memcached:
    * high thoughput, low latency; error-prone 
* MySQL: 
    * ACID, rich queries, decent performance; bad scalability
* Dynamo:
    * linear scalability, high availability; eventually consistency
* MongoDB:
    * high performance, high availability; eventually consistency

_How about Scalable Transactionl Datastore?_  
__Barely.__  
([F1][], [Scalaris][], [Spinnaker][])

[F1]: http://research.google.com/pubs/pub38125.html
[Scalaris]: http://code.google.com/p/scalaris/
[Spinnaker]: http://www.vldb.org/2011/files/slides/research15/rSession15-1.ppt
---
## Needs & Facts

* Why
    * Large Billing & Payments System:
        * PayPal, Alipay, ...
    * E-Commerce Website:
        * Amazon, Taobao, [12306.cn](http://www.12306.cn), ...
    * Online Advertising and Marketing:
        * Google, ...
* Why not
    * Complexity, extremely in large distributed environment.
    * High latency
    * Low thoughput
    * Error-prone
    * Hotspot & Contention

---
## Solutions

* [Sinfonia][]:
    * By Marcos K. Aguilera et al. in 2006 at HP Lab
    * Layered architecture based on Distributed Shared Address Space
    * Short-lived two phrase commit protocol
    * Strong consistency, good scalability, fault-tolerance, decent performance
* [Calvin][]
    * By Alexander Thomson et al. in 2012 at Yale University
    * Transaction scheduling and data replication middleware
    * Rearrange transactions in deterministic order
    * Strong consistency, linear scalability, fault-tolerance, high thoughput, affordable latency

[Sinfonia]: http://www.hpl.hp.com/research/ispil/projects/sinfonia/index.html
[Calvin]: http://dl.acm.org/citation.cfm?id=2213838
---
## Sinfonia

---
<dot>
    digraph a { a -> b;}
</dot>

<!-- 
    vim: filetype=markdown
-->
