import geni.portal as portal
import geni.rspec.pg as pg
import geni.rspec.igext as IG

pc = portal.Context()
request = pc.makeRequestRSpec()
tourDescription = \
"""
This profile provides the template for a Hortonworks cluster with one 
namenode and three datanode.
"""
tour = IG.Tour()
tour.Description(IG.Tour.TEXT,tourDescription)
request.addTour(tour)

prefixForIP = "192.168.1."
link = request.LAN("lan")

for i in range(4):
  if i == 0:
    node = request.XenVM("namenode")
  else:
    node = request.XenVM("datanode-" + str(i))
  node.cores = 4
  node.ram = 4096
  node.routable_control_ip = "true"  
  node.disk_image = "urn:publicid:IDN+emulab.net+image+emulab-ops:CENTOS7-64-STD"
  iface = node.addInterface("if" + str(i))
  iface.component_id = "eth1"
  iface.addAddress(pg.IPv4Address(prefixForIP + str(i + 1), "255.255.255.0"))
  link.addInterface(iface)

pc.printRequestRSpec(request)
