# RV1106_Linux
Linux test for RV1106 dev board

## v4l2
> class v4l2CapPicTool is a tool for capturing picture;

`arm-linux-gnueabihf-g++ v4l2CapPicTool.cpp captureJpgApp.cpp -o captureJpgApp`

### time cost 
```
capture time spent: 0.02077100 sec
save time spent: 0.00305000 sec
capture time spent: 0.01992500 sec
save time spent: 0.00133800 sec
capture time spent: 0.01959500 sec
save time spent: 0.00224100 sec
capture time spent: 0.02186700 sec
save time spent: 0.00239300 sec
capture time spent: 0.01873900 sec
save time spent: 0.00181500 sec
capture time spent: 0.01912700 sec
save time spent: 0.00134600 sec
capture time spent: 0.01973400 sec
save time spent: 0.00308300 sec
capture time spent: 0.01959300 sec
save time spent: 0.00237300 sec
capture time spent: 0.01859000 sec
save time spent: 0.00310700 sec
capture time spent: 0.01985500 sec
save time spent: 0.00120300 sec
capture time spent: 0.01963000 sec
save time spent: 0.00225200 sec
capture time spent: 0.02174400 sec
save time spent: 0.00238600 sec
capture time spent: 0.01873300 sec
save time spent: 0.00179200 sec
```
So, frequency of capture with usb camera can reach 50Hz.
