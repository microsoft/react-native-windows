# This region is important to match the org we run the test for i.e:
#   https://dev.azure.com/ms/_settings/organizationOverview
#   https://dev.azure.com/office/_settings/organizationOverview
$location='westus2'
$resourceGroup='rnw-ci'
$vmName=$resourceGroup + '-vm'

$storageSku='Premium_LRS'
$vmSku='Standard_D4as_v4'

$subscription='8735cced-c5b3-4da5-867b-2c3f680273c8'
$adminUser='CxeAdmin'
$adminPassword='CXE!#%' + (& uuidgen)
$adminSecurePassword  = ConvertTo-SecureString -String $adminPassword -AsPlainText -Force
$adminCreds = New-Object System.Management.Automation.PSCredential ($adminUser, $adminSecurePassword)
Write-Host "PWD: $adminPassword"

# Make sure  you log into azure first by running:
#   az login
# Set the default subscription
az account set -s $subscription

az group create --location $location --name $resourceGroup

# Setup the networking stack
az network vnet create --resource-group $resourceGroup --name ($resourceGroup + '-vnet') --subnet-name ($resourceGroup + '-subnet')
az network public-ip create --resource-group $resourceGroup --name ($resourceGroup + '-ipV4')
az network nsg create --resource-group $resourceGroup --name ($resourceGroup + '-netSg')
az network nic create --resource-group $resourceGroup --name ($resourceGroup + '-nic') --vnet-name ($resourceGroup + '-vnet') --subnet ($resourceGroup + '-subnet') --network-security-group ($resourceGroup + '-netSg') --public-ip-address ($resourceGroup + '-ipV4')

# Create a virtual machine. 
az vm create --resource-group $resourceGroup --name $vmName --location $location --nics ($resourceGroup + '-nic') --image MicrosoftWindowsServer:WindowsServer:2019-Datacenter:latest --size $vmSku --storage-sku $storageSku --admin-username $adminUser --admin-password $adminPassword --generate-ssh-keys --output json --verbose
az vm open-port --port 3389 --resource-group  $resourceGroup --name $vmName

# !!!!!!!!!!!!!!!!!!!!!!!
# Manual Steps
# !!!!!!!!!!!!!!!!!!!!!!!

# Unfortunately setting up teh machie is to slow to use
#      az vm run-command 
# because that has a timeout of 1 hour and it takes longer than that.
# so we'll have to remote into the box. 

# run: `mstsc /v:<publicIp>`
# the publicIp was printed when running `az vn create ...`

# 1. Remote into the box
#    1. The username and password are printed at the start of this script
# 1. Disable enhanced IE security
#    1. When you log in for the first time windows will launch the Server Manager Dashboard
#    1. Click "Local Servers" on the left side of the dashboard
#    1. Click on 'On' in the right column next to 'IE Enhanced Security configuration'
#    1. Select 'Off' for both options.
#    1. Click on 'Ok' to close the dialog
#    1. Close the Server Manager Dashboard
# 1. Download setup script from `https://raw.githubusercontent.com/microsoft/react-native-windows/master/vnext/Scripts/rnw-dependencies.ps1` to: `c:\warmup\rnw-dependencies.ps1`
# 1. Run downloaded script as admin:
#       `.\rnw-dependencies.ps1 -Install -NoPrompt buildlab`

# Reboot the VM for Visual Studio
az vm restart --resource-group $resourceGroup --name $vmName

# !!!!!!!!!!!!!!!!!!!!!!!
# Manual Steps
# !!!!!!!!!!!!!!!!!!!!!!!

# Warm up the vm
# 1. Remote into the box
#    1. The username and password are printed at the start of this script
# 1. Download the warmup script from `https://raw.githubusercontent.com/microsoft/react-native-windows/master/vnext/Scripts/Pipeline.WarmupVm.ps1` to `c:\warmup\Pipeline.WarmupVm.ps1`
# 1. Run the warmup script

# Shut down the VM and create a 'backup copy' of the hdd in case we need to bring up the VM again.
az vm stop --resource-group $resourceGroup --name $vmName

# Create SnapShot of the VHD in case we need to add software
#TODO Automate, used UI for now.
# 1: az vm show --resource-group $resourceGroup --name $vmName
# 2: az vm shapshot create .....

az vm start --resource-group $resourceGroup --name $vmName
mstsc /v:<ip>

# !!!!!!!!!!!!!!!!!!!!!!!
# Manual Steps
# !!!!!!!!!!!!!!!!!!!!!!!

# Prep image
# 1. Launch admin shell
# 1. C:\Windows\System32\sysprep\sysprep.exe /generalize /oobe /shutdown
# 1. Wait for the VM to be shut down...

# Prep image
az vm deallocate --resource-group  $resourceGroup --name $vmName 
az vm generalize --resource-group  $resourceGroup --name $vmName 
az image create  --resource-group  $resourceGroup --name $($resourceGroup + '-img') --source $vmName --location $location

# Create Scale Set
az vmss create --resource-group $resourceGroup --name ($resourceGroup + '-vmss') --image ($resourceGroup + '-img') --admin-username $adminUser --admin-password $adminPassword --instance-count 2 --disable-overprovision --upgrade-policy-mode manual --load-balancer '""' --storage-sku $storageSku --vm-sku $vmSku