# Build Scalable Transactional Datastore

<tianzhongbo@douban.com>  
2012-06-13

---
## Datastore Restrictions

* CAP Theorem
    * Consistency: 
        * all nodes see the same data at the same time.
    * Availability: 
        * a guarantee that every request receives a response about whether it was successful or failed.
    * Partition tolerance: 
        * the system continues to operate despite arbitrary message loss or failure of part of the system.
* Trade-offs
    * Performance
    * Scalability
    * Transactions
    * ...

---
## Datastore Overview

* Redis, Memcached:
    * high thoughput, low latency; error-prone.
* MySQL: 
    * ACID, rich queries, decent performance; bad scalability.
* Dynamo:
    * linear scalability, high availability; eventually consistency.
* MongoDB:
    * high performance, high availability; eventually consistency.

_How about Scalable Transactionl Datastore?_  
__Barely nothing.__  
([F1][], [Scalaris][], [Spinnaker][])

[F1]: http://research.google.com/pubs/pub38125.html
[Scalaris]: http://code.google.com/p/scalaris/
[Spinnaker]: http://www.vldb.org/2011/files/slides/research15/rSession15-1.ppt
---
## Needs & Facts

* Why?
    * Large Billing & Payments System:
        * PayPal, Alipay, ...
    * E-Commerce Website:
        * Amazon, Taobao, [12306.cn](http://www.12306.cn), ...
    * Online Advertising and Marketing:
        * Google, ...
* Why not?
    * Complexity, extremely in large distributed environment.
    * High latency
    * Low thoughput
    * Error-prone
    * Hotspot & Contention

---
## Solutions

* [Sinfonia][]:
    * By Marcos K. Aguilera et al. in 2006 at HP Lab.
    * Layered architecture based on Distributed Shared Address Space.
    * Short-lived two phrase commit protocol.
    * Strong consistency, good scalability, fault-tolerance, decent performance.
* [Calvin][]
    * By Alexander Thomson et al. in 2012 at Yale University.
    * Transaction scheduling and data replication middleware.
    * Rearrange transactions in deterministic order.
    * Strong consistency, linear scalability, fault-tolerance, high thoughput, affordable latency.

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
        label="read";
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
            label = "<f0> mem-id|<f1> addr|<f2> len|<f3> data"
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
## Calvin Overview

* Implemented as Middleware
    * _sequencer_: divide transaction into time-frames, and sort.
    * _scheduler_: transaction execution.
    * _storage_: provide CRUD interface.
<dot>
digraph G {
    size="10,4"
        concentrate=true;  
    paxos1[label="paxos", color="invis"];
    paxos2[label="paxos", color="invis"];
    subgraph cluster_0{
        label="replica A";
        subgraph cluster_1 {
            label="partition";
            style=filled;
            color=lightgrey;
            node [style=filled,color=white];
            seq1 [label="sequencer" shape="rect"];
            sche1 [label="scheduler" shape="rect"];
            store1 [label="storage"];
            seq1 -> sche1;
            sche1 -> store1;
            store1 -> sche1;
        }
        subgraph cluster_2 {
            label="partition";
            style=filled;
            color=lightgrey;
            node [style=filled,color=white];
            seq2 [label="sequencer" shape="rect"];
            sche2 [label="scheduler" shape="rect"];
            store2 [label="storage"];
            seq2 -> sche2;
            sche2 -> store2;
            store2 -> sche2;
        }    
        seq1 -> sche2;
        seq2 -> sche1;
        sche1 -> sche2;
        sche2 -> sche1;
    }
    subgraph cluster_3{
        label="replica B";
        subgraph cluster_4 {
            label="partition";
            style=filled;
            color=lightgrey;
            node [style=filled,color=white];
            seq4 [label="sequencer" shape="rect"];
            sche4 [label="scheduler" shape="rect"];
            store4 [label="storage"];
            seq4 -> sche4;
            sche4 -> store4;
            store4 -> sche4;
        }
        subgraph cluster_5 {
            label="partition";
            style=filled;
            color=lightgrey;
            node [style=filled,color=white];
            seq5 [label="sequencer" shape="rect"];
            sche5 [label="scheduler" shape="rect"];
            store5 [label="storage"];
            seq5 -> sche5;
            sche5 -> store5;
            store5 -> sche5;
        }    
        seq4 -> sche5;
        seq5 -> sche4;
        sche4 -> sche5;
        sche5 -> sche4;
    }

    paxos1 -> seq1;
    paxos2 -> seq2;
    paxos1 -> seq4;
    paxos2 -> seq5;
}
</dot>
---
## Calvin in Detail

* Paxos-based Synchronous Replication between Replicas
* Deterministic Concurrency Control
* Transaction Reordering
    * Ordered locking
    * Execution to completion
* Transaction execution in Schedulers:
    1. Read/write set analysis.
    1. Perform local reads.
    1. Serve remote reads.
    1. Collect remote read results.
    1. Transaction logic execution and applying writes.

---
## Drawbacks of Calvin

* High latency
* Transaction analysis required before execution
* Dependent Transaction and Optimistic Lock Location Prediction
* Slow machines & execution skew
* Suffering with long transactions
* Complex
* Contention sensitive

---
## Integrate the two models

* Minitransaction suits Calvin well:
    * Read/write sets predeclared
    * Short-lived
* Lock ordering improve thoughput in Sinfonia
* Adopt Sinfonia architecture to simplify Calvin
* Implemented as two counterpart policies:
    * Sinfonia: fast fail, lower thoughput
    * Calvin: high latency, high thoughput
* Separated storage layer for flexibility

---
## Doufu: scalable transactional datastore

<dot>
digraph G {
    size = "10,5"
    subgraph cluster_sql {
        label="SQL Parser Layer";
        sql1 [label="Parser", shape="rect"];
        sql2 [label="Parser", shape="rect"];
    }

    subgraph cluster_data {
        label="Distributed Filesystem";
        data1 [label="Append-Only File"];
        data2 [label="Append-Only File"];
    }

    subgraph cluster_index {
        label="Distributed Index layer";
        index1 [label="Index Maitainer", shape="rect"];
        index2 [label="Index Maintainer", shape="rect"];
    }

    subgraph cluster_sinfonia {
        label="Sinfonia";
        style=filled;
        color=lightgrey;
        subgraph cluster_coordinator {
            label="Coordinator Layer";
            coor1 [label="Coordinator", shape="rect"];
            coor2 [label="Coordinator", shape="rect"];
        }

        subgraph cluster_barrier {
            label="Barrier Layer";
            barr1 [label="Barrier", shape="rect"];
            barr2 [label="Barrier", shape="rect"];
        }
    }
    
    subgraph cluster_calvin {
        label="Calvin";
        style=filled;
        color=lightgrey;
        subgraph cluster_sequencer {
            label="Sequencer Layer";
            seq1 [label="Sequencer", shape="rect"];
            seq2 [label="Sequencer", shape="rect"];
        }

        subgraph cluster_scheduler {
            label="Scheduler Layer";
            sche1 [label="Scheduler", shape="rect"];
            sche2 [label="Scheduler", shape="rect"];
        }
    }

    subgraph cluster_memnode {
        label="Memory Nodes";
        mem1 [label="Memnode"];
        mem2 [label="Memnode"];
    }

    sql1 -> data1;
    sql2 -> data1;
    sql2 -> index1;
    sql1 -> index1;
    sql2 -> index2;
    index1 -> coor1;
    index1 -> coor2;
    index2 -> seq1;
    index2 -> seq2;
    coor1 -> barr1;
    coor1 -> barr2;
    coor2 -> barr2;
    seq1 -> sche1;
    seq1 -> sche2;
    seq2 -> sche1;
    seq2 -> sche2;
    barr1 -> mem1;
    barr2 -> mem2;
    sche1 -> mem1;
    sche2 -> mem2;
    index2 -> mem2 [label="Dirty Read"];
    barr2 -> data2 [color="invis"];
}
</dot>

---
## Designing Doufu

* Named after "豆腐", as codename.
* Separated memory nodes used.
    * Simplify and clarify interface.
    * Able to use existing facility, such as redis.
    * High availability required.
    * Extra latency.
* Key-value Table instead of Linear Address Space.
* Barrier layer manage read and write locks.
* Minitransaction as unified interface.
    * Hide model differences.
    * compares lead to transaction restart.
* Dirty read to reduce network trip.
* Append-Only data file make transactions easy, and is needed by MVCC.

---
## Status of Doufu

* Just in my mind.
* Planning as part-time project.
* Some reference projects:
    * [py-concerto][]: Sinfonia implemented in Python
    * [SinfoniaEx][]: part of Qstream project, written in C++, with distributed B-Tree.
    * Mongodb: for B-tree implementation and append-only data file.
* To write in C++
* Finding unit testing framework, network simulator, and etc.

[py-concerto]: http://code.google.com/p/py-concerto/
[SinfoniaEx]: http://svn.qstream.org/qstream/branches/ast_failover_branch/sinfonia/

---
# The END
<section>
# Q & A
</section>
<section>
# Thanks For Your Time
</section>

<!-- 
vim: filetype=markdown
-->
