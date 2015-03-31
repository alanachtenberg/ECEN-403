package alan.cardiac;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.widget.Toast;

public class BluetoothComm extends Service {

    public BluetoothComm() {
    }

    @Override
    public IBinder onBind(Intent intent) {
        // TODO: Return the communication channel to the service.
        throw new UnsupportedOperationException("Not yet implemented");
    }

    @Override
    public void onCreate() {

        Toast.makeText(this, "Bluetooth Comm Service Created", Toast.LENGTH_LONG).show();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        // For time consuming an long tasks you can launch a new thread here...
        // Do your Bluetooth Work Here
        Toast.makeText(this, "Bluetooth Comm Service Started", Toast.LENGTH_LONG).show();


        /*START_NOT_STICKY Tells os to not recreate service if it is stopped in the event to free memmory.
        * In other words, we will let the user trigger starting a new service in the event
        * that it is stopped
         */
        return START_NOT_STICKY;

    }

    @Override
    public void onDestroy() {
        Toast.makeText(this, "Bluetooth Comm Service Destroyed", Toast.LENGTH_LONG).show();

    }
}
