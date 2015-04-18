package alan.cardiac;

import android.content.SharedPreferences;
import android.os.Bundle;
import android.preference.EditTextPreference;
import android.preference.Preference;
import android.preference.PreferenceFragment;
import android.preference.PreferenceGroup;


/**
 * Use the {@link SettingsFragment#newInstance} factory method to
 * create an instance of this fragment.
 */
public class SettingsFragment extends PreferenceFragment implements SharedPreferences.OnSharedPreferenceChangeListener {


    /**
     * Use this factory method to create a new instance of
     * this fragment using the provided parameters.
     *
     * @return A new instance of fragment SettingsFragment.
     */
    public static SettingsFragment newInstance() {
        SettingsFragment fragment = new SettingsFragment();
        Bundle args = new Bundle();
        fragment.setArguments(args);
        return fragment;
    }

    public SettingsFragment() {
        // Required empty public constructor
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        addPreferencesFromResource(R.xml.fragment_preference);
        //register this on the shared preference change listener
        //allows update of summary when selection is changed
        getPreferenceScreen().getSharedPreferences().registerOnSharedPreferenceChangeListener(this);

        //initialize edit text preference summary to display value
        for (int i = 0; i < getPreferenceScreen().getPreferenceCount(); ++i) {//list of preference groups
            Preference preference = getPreferenceScreen().getPreference(i);
            if (preference instanceof PreferenceGroup) {
                PreferenceGroup preferenceGroup = (PreferenceGroup) preference;
                for (int j = 0; j < preferenceGroup.getPreferenceCount(); ++j) {//list of preferences
                    if (preferenceGroup.getPreference(j) instanceof EditTextPreference) {
                        setSummaryPreference(preferenceGroup.getPreference(j));//set summary to its value
                    }
                }
            }
        }
    }

    @Override
    //update the summary when changing preference value
    public void onSharedPreferenceChanged(SharedPreferences sharedPreferences, String key) {
        Preference preference = findPreference(key);
        setSummaryPreference(preference);
    }
    private void setSummaryPreference(Preference preference){
        if (preference instanceof EditTextPreference) {
            EditTextPreference editPreference = (EditTextPreference) preference;
            if (editPreference.getText() != null) {
                if (editPreference.getKey().endsWith("pass"))
                    editPreference.setSummary(editPreference.getText().replaceAll(".", "*"));
                else
                    editPreference.setSummary(editPreference.getText());

            }
        }
    }
}
