package alan.cardiac;

import android.app.Service;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.os.IBinder;
import android.preference.PreferenceManager;
import android.widget.Toast;

import java.io.IOException;
import java.util.Set;

public class BluetoothComm extends Service {
    private SharedPreferences mSP;
    private BluetoothAdapter mBluetoothAdapter;
  //private BroadCastReciever mReciever defined later
    private Set<BluetoothDevice> pairedDevices;
    private String targetDeviceName="default";
    private Boolean targetDevicePaired=false;
    private BluetoothDevice targetDevice;
    private BtCommThread btCommThread;
    private Boolean isStarted=false;//allow only one connection at a time
    public BluetoothComm() {
    }

    @Override
    public IBinder onBind(Intent intent) {
        // TODO: Return the communication channel to the service.
        throw new UnsupportedOperationException("Not yet implemented");
    }

    @Override
    public void onCreate() {
        mBluetoothAdapter=BluetoothAdapter.getDefaultAdapter();
        mSP = PreferenceManager.getDefaultSharedPreferences(getBaseContext());
        // Register the BroadcastReceiver for when a bluetooth device is found
        IntentFilter filter = new IntentFilter(BluetoothDevice.ACTION_FOUND);
        registerReceiver(mReceiver, filter); // Don't forget to unregister during onDestroy
        filter= new IntentFilter(BluetoothDevice.ACTION_BOND_STATE_CHANGED);
        registerReceiver(mReceiver,filter);
        Toast.makeText(this, "Bluetooth Comm Service Created", Toast.LENGTH_LONG).show();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        if (!isStarted) {
            isStarted=true;
            Toast.makeText(this, "BluetoothComm Service Started", Toast.LENGTH_SHORT).show();
            retrievePreferences();//gets target device name
            targetDevicePaired = checkForTargetDevice();
            if (!targetDevicePaired) {
                discoverDevice();//initiate discovery
            } else if (btCommThread == null || !btCommThread.isAlive()) {
                try {
                    btCommThread = new BtCommThread(targetDevice, getApplicationContext());
                    btCommThread.start();//
                } catch (IOException e) {
                    e.printStackTrace();
                    btCommThread = null;//must have been a problem with Thread constructor, so get rid of instance
                    Toast.makeText(this, "Connection Failed", Toast.LENGTH_LONG).show();
                    stopSelf();//stop the bluetooth service, let user have a chance to update settings
                }
            } else {
                Toast.makeText(this, "Bluetooth Device Already Connected", Toast.LENGTH_LONG).show();
            }
        /*START_NOT_STICKY Tells os to not recreate service if it is stopped in the event to free memmory.
        * In other words, we will let the user trigger starting a new service in the event
        * that it is stopped
         */
        }
            return START_NOT_STICKY;
    }

    @Override
    public void onDestroy() {
        try {
            unregisterReceiver(mReceiver);

            //dispose of connection thread
            if (btCommThread != null) {
                btCommThread.interrupt();//signals for thread to stop execution as soon as possible
                btCommThread.close();//closes socket connection to device, must be done to cancel any blocking reads that might prevent interruption
                btCommThread.join();//wait for thread to complete execution
            }
            Toast.makeText(this, "Bluetooth Comm Service Destroyed", Toast.LENGTH_SHORT).show();
        }
        catch (InterruptedException e) {
                e.printStackTrace();
        }
    }

    private Boolean checkForTargetDevice(){
        pairedDevices = mBluetoothAdapter.getBondedDevices();
        // If there are paired devices
        if (pairedDevices.size() > 0) {
            // Loop through paired devices
            for (BluetoothDevice device : pairedDevices) {
                // Check if our device is paired already
                if(device.getName().equals(targetDeviceName)) {//if paired device name is the same as our preference
                    targetDevice = device;
                    return true;
                }
            }
        }
        return false;
    }

    private void discoverDevice(){
        if(mBluetoothAdapter.isDiscovering())
            mBluetoothAdapter.cancelDiscovery();
        mBluetoothAdapter.startDiscovery();
    }

    private void retrievePreferences(){
        targetDeviceName=mSP.getString("bt_device_name","Udoo");
    }


    // Create a BroadcastReceiver for ACTION_FOUND
    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            // When discovery finds a device
            if (BluetoothDevice.ACTION_FOUND.equals(action)) {
                // Get the BluetoothDevice object from the Intent
                BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                // determine if device is our desired target
                if (device != null && device.getName() != null && device.getName().equals(targetDeviceName)) {
                    targetDevice = device;
                    targetDevicePaired = targetDevice.createBond();
                }
            } else if (BluetoothDevice.ACTION_BOND_STATE_CHANGED.equals(action)) {
                if (intent.getParcelableExtra(BluetoothDevice.EXTRA_BOND_STATE).equals(BluetoothDevice.BOND_BONDED)
                        && intent.getParcelableExtra(BluetoothDevice.EXTRA_PREVIOUS_BOND_STATE).equals(BluetoothDevice.BOND_BONDING)) {
                    if (btCommThread == null || btCommThread.isAlive() == false) {
                        try {
                            btCommThread = new BtCommThread(targetDevice, context.getApplicationContext());
                            btCommThread.start();//start connection thread
                        }
                        catch(IOException e){
                            e.printStackTrace();
                            btCommThread=null;//must have been a problem with Thread constructor, so get rid of instance
                            Toast.makeText(context,"Connection Failed",Toast.LENGTH_LONG).show();
                            stopSelf();//stop the bluetooth service, let user have a chance to update settings
                        }
                    }
                }
            }
        }
    };



}
