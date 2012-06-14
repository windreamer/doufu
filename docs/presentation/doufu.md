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
## Sinfonia in Detail
* Shared Linear Address Space
* Minitransaction  
<dot>
digraph G {
    size="10,4";
    subgraph cluster_0{
        label="read"
            node1 [
            label = "<f0> mem-id|<f1> addr|<f2> len"
            shape = "record"
            ];
        node2 [
            label = "<f0> mem-id|<f1> addr|<f2> len"
            shape = "record"
            ];
    }
    subgraph cluster_1{
        label="compare";
        node3 [
            label = "<f0> mem-id|<f1> addr|<f2> len|<f3> data"
            shape = "record"
            ];
        node3 [
            label = "<f0> mem-id|<f1> addr|<f2> len|<f3> data"
            shape = "record"
            ];
    }
    subgraph cluster_3{
        label="write";
        node5 [
            label = "<f0> mem-id|<f1> addr|<f2> len|<f3> data"
            shape = "record"
            ];
        node6 [
            label = "<f0> write|<f1> id|<f2> addr|<f3> len|<f4> data"
            shape = "record"
            ];
    }
}
</dot>
* Short-lived 2PC protocol  
<dot>
digraph G {
    size="6,10"
    rankdir="LR"
    subgraph cluster_0 {
        color=invis;

        subgraph cluster_1 {
            style=filled;
            color=lightgrey;
            node [style=filled,color=white];
            Master1 -> Slave1;
            label = "replica 1";
        }

        subgraph cluster_2 {
            style=filled;
            color=lightgrey;
            node [style=filled,color=white];
            Master2 -> Slave2;
            label = "replica 2";
        }

        subgraph cluster_3 {
            style=filled;
            color=lightgrey;
            node [style=filled,color=white];
            Master3 -> Slave3;
            label = "replica 3";
        }
    }

    subgraph cluster {
        color=invis;
        rank="same" {
            Coordinator1 [label="Coordinator"]; 
            Coordinator2 [label="Coordinator"];
        }
    }

    Coordinator1 -> Master1 [label="1. Prepare"];
    Coordinator1 -> Master3 [label="1. Prepare"];
    Master1->Coordinator1 [label="2. OK/Fail"];
    Master3->Coordinator1 [label="2. OK/Fail"];
    Coordinator1 -> Master1 [label="3. Commit/About"];
    Coordinator1 -> Master3 [label="3. Commit/About"];

}
</dot>

---
## Sinfonia as Building Block

* Use Minitransaction to implement atomic write, CAS, swap, and etc.
* Optimistic Transaction
* [Scalable Distributed B-Tree](http://dl.acm.org/citation.cfm?id=1453922)
* Migration, Load Balancing and Locality
* Contention & Caching
* Replication
* Coordinator Crash Recovery

---
## Calvin in Detail

* Deterministic Concurrency Control

<!-- 
    vim: filetype=markdown
-->
