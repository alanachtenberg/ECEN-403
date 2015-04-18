package alan.cardiac;

import android.app.Service;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.IBinder;
import android.preference.PreferenceManager;
import android.widget.Toast;

import java.io.IOException;
import java.net.Socket;

/**
 * Created by Alan on 4/13/2015.
 */
public class TcpService extends Service {
    private SharedPreferences mSP;

    private Socket clientSocket;
    private Boolean connected;
    private String host;
    private String port;
    private String pass;
    Thread tcpCommThread;

    @Override
    public IBinder onBind(Intent intent) {
        // TODO: Return the communication channel to the service.
        throw new UnsupportedOperationException("Not yet implemented");
    }

    @Override
    public void onCreate() {
       Toast.makeText(this, "Networking Service Created", Toast.LENGTH_LONG).show();
       mSP=PreferenceManager.getDefaultSharedPreferences(getBaseContext());
       connected=false;
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        host=(String)mSP.getString("host_address","localhost");
        port=(String)mSP.getString("port_num","1234");
        pass=(String)mSP.getString("server_pass","CARS");
        if (!connected){
            try {
                //TODO fix exception error
                connected=true;
                tcpCommThread = new Thread(new TcpCommThread(host,port,pass));
                tcpCommThread.start();
            } catch (IOException e) {
                connected=false;
                e.printStackTrace();
            }

        }

        /*START_NOT_STICKY Tells os to not recreate service if it is stopped in the event to free memmory.
        * In other words, we will let the user trigger starting a new service in the event
        * that it is stopped
         */
        return START_NOT_STICKY;
    }

    @Override
    public void onDestroy() {

    }

}
