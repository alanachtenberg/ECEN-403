package alan.cardiac;

import android.app.Activity;
import android.app.Fragment;
import android.bluetooth.BluetoothAdapter;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;

public class HomeFragment extends Fragment {
    private View view;
    private Button connectBlueToothButton;
    private Button connectServerButton;
    private BluetoothAdapter mBluetoothAdapter;
    private static final int REQUEST_ENABLE_BT=1;

    public static HomeFragment newInstance() {
        HomeFragment fragment = new HomeFragment();
        Bundle args = new Bundle();
        fragment.setArguments(args);
        return fragment;
    }

    public HomeFragment(){
        //empty public constructor
    }
    @Override
    public void onCreate(Bundle savedInstanceState){
        super.onCreate(savedInstanceState);
    }
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        view=inflater.inflate(R.layout.fragment_home, container, false);

        connectBlueToothButton = (Button) view.findViewById(R.id.connect_bluetooth_button);
        connectBlueToothButton.setOnClickListener(connectBlueToothListener);

        connectServerButton = (Button) view.findViewById(R.id.connect_server_button);
        connectServerButton.setOnClickListener(connectServerListener);

        return view;
    }

    private View.OnClickListener connectBlueToothListener= new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
            if (mBluetoothAdapter == null) {
                Log.d("CARdiac", "device does not support Bluetooth");// Device does not support Bluetooth
            }
            if (!mBluetoothAdapter.isEnabled()) {
                Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
               startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
            }
            else{
                StartBluetoothService();
            }
        }
    };
    private View.OnClickListener connectServerListener= new View.OnClickListener(){
        @Override
        public void onClick(View v){
            StartTcpService();
        }
    };

    public void StartBluetoothService(){
        getActivity().startService(new Intent(getActivity(),BluetoothService.class));
    }

    public void StartTcpService(){
        getActivity().startService(new Intent(getActivity(),TcpService.class));
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode==REQUEST_ENABLE_BT){
            if (resultCode== Activity.RESULT_OK)
                StartBluetoothService();
        }
        else
            super.onActivityResult(requestCode, resultCode, data);
    }
}