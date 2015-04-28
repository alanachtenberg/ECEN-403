package alan.cardiac;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
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
    public static Boolean connected;
    private String host, port;
    private String user, pass;

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
        try {
            host=(String)mSP.getString("host_address","localhost");
            port=(String)mSP.getString("port_num","1234");
            pass=(String)mSP.getString("server_pass","CARS");
            user=(String)mSP.getString("user_name","Android");
            if (!connected) {
                ConnectivityManager connMgr = (ConnectivityManager)
                        getSystemService(Context.CONNECTIVITY_SERVICE);
                NetworkInfo networkInfo = connMgr.getActiveNetworkInfo();
                if (networkInfo != null && networkInfo.isConnected()) {
                    connected = true;
                    tcpCommThread = new Thread(new TcpCommThread(host, port, user, pass,getApplicationContext()));
                    tcpCommThread.start();
                } else {
                    //TODO make sure network connection is available
                }

            }
            /*START_NOT_STICKY Tells os to not recreate service if it is stopped in the event to free memmory.
            * In other words, we will let the user trigger starting a new service in the event
            * that it is stopped
             */
            return START_NOT_STICKY;
        } catch (IOException e) {
            connected=false;
            e.printStackTrace();
            stopSelf();//stop the TCP service, let user have a chance to update settings
        }
            return START_NOT_STICKY;
    }

    @Override
    public void onDestroy() {

    }

}
