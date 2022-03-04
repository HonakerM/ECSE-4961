
# Memory and Storage Performance Profiling

  

This is the third project for ECSE-4961. The goal of this project is to test and document the performance of our memory and storage devices.  All results, graphs, and excel files utilized in making this report can be found in the `/results` folder of this repository.


## Storage Configuration 
### Testing
For the storage portion of this project I will be utilizing [Flexible I/O Tester (fio)](https://fio.readthedocs.io/en/latest/fio_doc.html) running on Ubuntu 20.04.  I utilized this [fio job file](https://github.com/HonakerM/ECSE-4961/blob/main/project%203/fio_jobs.txt) which runs the following set of tests 


These tests are ran with the following global options which configures the jobs to utilize a file size of `1Gb` with a io block size of `4kb` , loop 5 times, write directly to the disk and utilize the `libaio` engine. To run this script on windows change `libaio` to `windowsaio`.
```
[global]
size=1g
bs=4k
filename=/tmp/fiotest.tmp
loops=5

;direct and libaio for engine
direct=1
ioengine=libaio
```

To execute the job file all you need to do is run the following command
```
fio fio_jobs.txt
```

### Disks
I will be testing two different storage configurations in this report. The first configuration is a set of 2 [HGST Ultrastar HDDs](https://www.amazon.com/HGST-Ultrastar-HUC109060CSS601-2-5-inch-Internal/dp/B07K8RXJJ1) configured in a RAID 1. These drives are interfaced with over a [HPE Smart Array P440 Controller](https://support.hpe.com/hpesc/public/docDisplay?docId=emr_na-c04491905) which contains a sizable cache for the drives to take advantage off. The second configuration utilizes the same drives but behind a 3 node Kubernetes cluster running [longhorn](https://longhorn.io/).  Longhorn utilizes realtime data replication to ensure high availability for cloud computing. However, as you will see later on this comes at a terrible cost to performance

## Storage Results And Analysis
### Bandwidth vs IOPS vs Latency 
As you can see in the above graphs as the queue depth grows bandwidth increases, IOPS increase, and also latency increases. While increasing bandwidth and IOPS are a good thing the increase in latency is not. Another thing to notice is that the both drives have a relatively constant average IOPS irregardless of weather they're right focused or read focused; however, longhorn's IOPS did max out around 19 IOPS for a queue depth of both `16` and `32`. And as expected directly writing on the raid 1 is significantly faster but longhorn's uptime might be worth the cost. Both of these drives are significantly slower than the Intel Data Center NVMe SSD D7-P5600. This is most likely due to utilizing SSD technology which has significantly higher IOPS than traditional HDD. The drive also utilizes NVMe which has a peak bandwidth of 32Gbs which is much higher than the SAS drives 6Gbs. The drive is also a enterprise grade caching drive that is designed to maximize for peak IOPS instead of the general purpose HDD drives. 




## Memory Configuration
### Testing
I utilized the [Intel Memory Checker](https://www.intel.com/content/www/us/en/developer/articles/tool/intelr-memory-latency-checker.html) to run my performance testing. To perform the tests I ran the following command:
```
./mlc
```
 
###  Memory Modules
I will be testing 2 different memory configurations the first are two sticks of HPE  `8Gb 2133MHz ECC DDR4` while the second is 2 DIMs of consumer grade `8Gb 2133MHz DDR4`.


### Memory Results

As you can see in the above to graphs as expected the commercial memory had a significantly higher peak bandwidth than the enterprise dims but that came a a cost of significantly higher latency. The enterprise module was able to deliver its max bandwidth without sacrificing latency. This is most likely due more advance stability and reliability tweaks that the enterprise market normally utilize. 
