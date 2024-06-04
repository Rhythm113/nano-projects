import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

public class ConfigParser {
    public static void parse(String filePath, Map<String, Object> map) {
        try (BufferedReader reader = new BufferedReader(new FileReader(filePath))) {
            String line;
            String currentSection = null;
            while ((line = reader.readLine()) != null) {
                line = line.trim();
                if (line.isEmpty() || line.startsWith("*")) {
                    continue; // Ignore empty lines and comments
                } else if (line.startsWith("[")) {
                    currentSection = line.substring(1, line.length() - 1);
                    map.put(currentSection, new HashMap<>());
                } else {
                    if (currentSection != null) {
                        int equalsIndex = line.indexOf("=");
                        if (equalsIndex != -1) {
                            String key = line.substring(0, equalsIndex).trim();
                            String value = line.substring(equalsIndex + 1).trim();
                            ((HashMap<String, Object>) map.get(currentSection)).put(key, value);
                        }
                    }
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static void encode(Map<String, Object> map, String filePath) {
        try (BufferedWriter writer = new BufferedWriter(new FileWriter(filePath))) {
            for (Map.Entry<String, Object> entry : map.entrySet()) {
                String section = entry.getKey();
                writer.write("[" + section + "]");
                writer.newLine();
                HashMap<String, Object> sectionMap = (HashMap<String, Object>) entry.getValue();
                for (Map.Entry<String, Object> item : sectionMap.entrySet()) {
                    String key = item.getKey();
                    Object value = item.getValue();
                    writer.write(key + "=" + value.toString());
                    writer.newLine();
                }
                writer.newLine();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
