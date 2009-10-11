/*
 *  test.c
 *  version_sorter
 *
 *  Created by K. Adam Christensen on 10/10/09.
 *  Copyright 2009. All rights reserved.
 *
 */

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include "cmockery.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/times.h>
#include "utils.h"
#include "strings.h"
#include "hash_test.h"
#include "version_sorter.h"

extern void parse_version_word(char *, StringLinkedList *);
extern void setup_version_regex(void);
extern char * create_flattened_version(StringLinkedList *, int);

static char *unsorted[] = {
    "1.0.9",        "1.0.10",       "1.10.1",
    "yui3-999",     "2.0",          "1.9.1",
    "yui3-990",     "3.1.4.2",      "1.0.9a",  
};

static char *expected_sorted[] = {
    "1.0.9",        "1.0.9a",       "1.0.10",
    "1.9.1",        "1.10.1",       "2.0", 
    "3.1.4.2",      "yui3-990",     "yui3-999",
};

static char *benchmark_list[] = { "yui3-571", "yui3-309", "yui3-1405", "yui3-1537", "yui3-440", "yui3-572", "yui3-1406", "yui3-1538", "yui3-441", "yui3-573", "yui3-1407", "yui3-1539", "yui3-310", "yui3-442", "yui3-574", "yui3-1408", "yui3-311", "yui3-443", "yui3-575", "yui3-1409", "yui3-312", "yui3-444", "yui3-576", "yui3-1540", "yui3-313", "yui3-445", "yui3-577", "yui3-1541", "yui3-314", "yui3-446", "yui3-1410", "yui3-578", "yui3-1542", "yui3-315", "yui3-447", "yui3-1411", "yui3-579", "yui3-1543", "3.0.0", "yui3-316", "yui3-448", "yui3-1412", "yui3-1544", "yui3-317", "yui3-449", "yui3-1413", "yui3-1545", "yui3-318", "yui3-1414", "yui3-1546", "yui3-580", "yui3-319", "yui3-1415", "yui3-1547", "yui3-581", "yui3-1416", "yui3-1548", "yui3-450", "yui3-582", "yui3-1417", "yui3-1549", "yui3-451", "yui3-583", "yui3-1418", "yui3-320", "yui3-452", "yui3-584", "yui3-1419", "yui3-321", "yui3-453", "yui3-585", "yui3-322", "yui3-454", "yui3-586", "yui3-1550", "yui3-323", "yui3-455", "yui3-587", "yui3-1551", "yui3-324", "yui3-456", "yui3-1420", "yui3-588", "yui3-1552", "yui3-325", "yui3-457", "yui3-1421", "yui3-589", "yui3-1553", "yui3-326", "yui3-458", "yui3-1422", "yui3-1554", "yui3-327", "yui3-459", "yui3-1423", "yui3-1555", "yui3-590", "yui3-328", "yui3-1424", "yui3-1556", "yui3-591", "yui3-329", "yui3-1425", "yui3-1557", "yui3-460", "yui3-592", "yui3-1426", "yui3-1558", "yui3-461", "yui3-593", "yui3-1427", "yui3-1559", "yui3-330", "yui3-462", "yui3-594", "yui3-1428", "yui3-331", "yui3-463", "yui3-595", "yui3-1429", "yui3-332", "yui3-464", "yui3-596", "yui3-1560", "yui3-333", "yui3-465", "yui3-597", "yui3-1561", "yui3-334", "yui3-466", "yui3-1430", "yui3-598", "yui3-1562", "yui3-335", "yui3-467", "yui3-1431", "yui3-599", "yui3-1563", "yui3-336", "yui3-1300", "yui3-468", "yui3-1432", "yui3-1564", "yui3-337", "yui3-1301", "yui3-469", "yui3-1433", "yui3-338", "yui3-1302", "yui3-1434", "yui3-339", "yui3-1303", "yui3-1435", "yui3-470", "yui3-1304", "yui3-1436", "yui3-471", "yui3-1305", "yui3-1437", "yui3-340", "yui3-472", "yui3-1306", "yui3-1438", "yui3-341", "yui3-473", "yui3-1307", "yui3-1439", "yui3-342", "yui3-474", "yui3-1308", "yui3-475", "yui3-1309", "yui3-343", "yui3-476", "yui3-1440", "yui3-344", "yui3-477", "yui3-1441", "yui3-345", "yui3-478", "yui3-1442", "yui3-346", "yui3-1310", "yui3-479", "yui3-1443", "yui3-900", "yui3-347", "yui3-1311", "yui3-1444", "yui3-901", "yui3-348", "yui3-1312", "yui3-1445", "yui3-902", "yui3-349", "yui3-1313", "yui3-1446", "yui3-903", "yui3-480", "yui3-1314", "yui3-1447", "yui3-904", "yui3-481", "yui3-1315", "yui3-1448", "yui3-905", "yui3-350", "yui3-482", "yui3-1316", "yui3-1449", "yui3-906", "yui3-351", "yui3-483", "yui3-1317", "yui3-907", "yui3-352", "yui3-484", "yui3-1318", "yui3-908", "yui3-353", "yui3-485", "yui3-1319", "yui3-909", "yui3-354", "yui3-486", "yui3-1450", "yui3-355", "yui3-487", "yui3-1451", "yui3-356", "yui3-1320", "yui3-488", "yui3-1452", "yui3-910", "yui3-357", "yui3-1321", "yui3-489", "yui3-1453", "yui3-911", "yui3-358", "yui3-1322", "yui3-1454", "yui3-912", "yui3-359", "yui3-1323", "yui3-1455", "yui3-490", "yui3-1324", "yui3-1456", "yui3-913", "yui3-491", "yui3-1325", "yui3-1457", "yui3-914", "yui3-360", "yui3-492", "yui3-1326", "yui3-1458", "yui3-915", "yui3-361", "yui3-493", "yui3-1327", "3.0.0pr2", "yui3-1459", "yui3-916", "yui3-362", "yui3-494", "yui3-1328", "yui3-917", "yui3-363", "yui3-495", "yui3-1329", "yui3-918", "yui3-364", "yui3-496", "yui3-1460", "yui3-919", "yui3-365", "yui3-497", "yui3-1461", "yui3-366", "yui3-1330", "yui3-498", "yui3-1462", "yui3-367", "yui3-1331", "yui3-499", "yui3-1463", "yui3-920", "yui3-1200", "yui3-368", "yui3-1332", "yui3-1464", "yui3-921", "yui3-1201", "yui3-369", "yui3-1333", "yui3-1465", "yui3-922", "yui3-1202", "yui3-1334", "yui3-1466", "yui3-923", "yui3-1203", "yui3-1335", "yui3-1467", "yui3-924", "yui3-1204", "yui3-1336", "yui3-1468", "yui3-925", "yui3-370", "yui3-1205", "yui3-1337", "yui3-1469", "yui3-926", "yui3-371", "yui3-1206", "yui3-1338", "yui3-927", "yui3-372", "yui3-1207", "yui3-1339", "yui3-928", "yui3-373", "yui3-1208", "yui3-929", "yui3-374", "yui3-1470", "yui3-1209", "yui3-375", "yui3-1471", "yui3-376", "yui3-1340", "yui3-1472", "yui3-930", "yui3-377", "yui3-1341", "yui3-1473", "yui3-931", "yui3-1210", "yui3-378", "yui3-1342", "yui3-1474", "yui3-800", "yui3-932", "yui3-1211", "yui3-379", "yui3-1343", "yui3-1475", "yui3-801", "yui3-933", "yui3-1212", "yui3-1344", "yui3-1476", "yui3-802", "yui3-934", "yui3-1213", "yui3-1345", "yui3-1477", "yui3-803", "yui3-935", "yui3-380", "yui3-1214", "yui3-1346", "yui3-1478", "yui3-804", "yui3-936", "yui3-381", "yui3-1215", "yui3-1347", "yui3-1479", "yui3-805", "yui3-937", "yui3-382", "yui3-1216", "yui3-1348", "yui3-806", "yui3-938", "yui3-383", "yui3-1349", "yui3-807", "yui3-939", "yui3-384", "yui3-1480", "yui3-1218", "yui3-808", "yui3-385", "yui3-1481", "yui3-1219", "yui3-809", "yui3-254", "yui3-386", "yui3-1350", "yui3-1482", "yui3-255", "yui3-387", "yui3-1351", "yui3-1483", "yui3-940", "yui3-256", "yui3-1220", "yui3-388", "yui3-1352", "yui3-1484", "yui3-941", "yui3-257", "yui3-1221", "yui3-389", "yui3-1353", "yui3-1485", "yui3-810", "yui3-942", "yui3-258", "yui3-1222", "yui3-1354", "yui3-1486", "yui3-811", "yui3-943", "yui3-259", "yui3-1223", "yui3-1355", "yui3-1487", "yui3-812", "yui3-944", "yui3-1224", "yui3-1356", "yui3-1488", "yui3-813", "yui3-945", "yui3-390", "yui3-1225", "yui3-1357", "yui3-1489", "yui3-814", "yui3-946", "yui3-391", "yui3-1226", "yui3-1358", "yui3-815", "yui3-947", "yui3-260", "yui3-392", "yui3-1227", "yui3-1359", "yui3-816", "yui3-948", "yui3-261", "yui3-393", "yui3-1490", "yui3-1228", "yui3-817", "yui3-949", "yui3-262", "yui3-394", "yui3-1491", "yui3-1229", "yui3-818", "yui3-263", "yui3-395", "yui3-1492", "yui3-819", "yui3-264", "yui3-396", "yui3-1360", "yui3-1493", "yui3-950", "yui3-265", "yui3-397", "yui3-1361", "yui3-1494", "yui3-951", "yui3-266", "yui3-1230", "yui3-398", "yui3-1362", "yui3-1495", "yui3-820", "yui3-952", "yui3-267", "yui3-1231", "yui3-399", "yui3-1363", "yui3-1496", "yui3-821", "yui3-953", "yui3-1100", "yui3-268", "yui3-1232", "yui3-1364", "yui3-1497", "yui3-822", "yui3-954", "yui3-1101", "yui3-269", "yui3-1233", "yui3-1365", "yui3-823", "yui3-955", "yui3-1102", "yui3-1234", "yui3-1366", "yui3-1498", "yui3-824", "yui3-956", "yui3-1103", "yui3-1235", "yui3-1367", "yui3-1499", "yui3-825", "yui3-957", "yui3-270", "yui3-1104", "yui3-1236", "yui3-1368", "yui3-826", "yui3-958", "yui3-271", "yui3-1105", "yui3-1237", "yui3-1369", "yui3-827", "yui3-959", "yui3-272", "yui3-1106", "yui3-1238", "yui3-828", "yui3-273", "yui3-1107", "yui3-1239", "yui3-829", "yui3-274", "yui3-1370", "yui3-1108", "yui3-275", "yui3-1371", "yui3-1109", "yui3-960", "yui3-276", "yui3-1240", "yui3-1372", "yui3-961", "yui3-277", "yui3-1241", "yui3-1373", "yui3-830", "yui3-962", "yui3-1110", "yui3-278", "yui3-1242", "yui3-1374", "yui3-831", "yui3-963", "yui3-1111", "yui3-279", "yui3-1243", "yui3-1375", "yui3-700", "yui3-832", "yui3-964", "yui3-1112", "yui3-1244", "yui3-1376", "yui3-701", "yui3-833", "yui3-965", "yui3-1113", "yui3-1245", "yui3-1377", "yui3-702", "yui3-834", "yui3-966", "yui3-280", "yui3-1114", "yui3-1246", "yui3-1378", "yui3-703", "yui3-835", "yui3-967", "yui3-281", "yui3-1115", "yui3-1247", "yui3-1379", "yui3-704", "yui3-836", "yui3-968", "yui3-282", "yui3-1116", "yui3-1248", "yui3-705", "yui3-837", "yui3-969", "yui3-283", "yui3-1117", "yui3-1249", "yui3-706", "yui3-838", "yui3-284", "yui3-1380", "yui3-1118", "yui3-707", "yui3-839", "yui3-285", "yui3-1381", "yui3-1119", "yui3-970", "yui3-708", "yui3-286", "yui3-1250", "yui3-1382", "yui3-971", "yui3-709", "yui3-287", "yui3-1251", "yui3-1383", "yui3-840", "yui3-972", "yui3-288", "yui3-1252", "yui3-1384", "yui3-841", "yui3-973", "yui3-1120", "yui3-289", "yui3-1253", "yui3-1385", "yui3-710", "yui3-842", "yui3-974", "yui3-1121", "yui3-1254", "yui3-1386", "yui3-711", "yui3-843", "yui3-975", "yui3-1122", "yui3-1255", "yui3-1387", "yui3-712", "yui3-844", "yui3-976", "yui3-1123", "yui3-1256", "yui3-1388", "yui3-713", "yui3-845", "yui3-977", "yui3-290", "yui3-1124", "yui3-1257", "yui3-1389", "yui3-714", "yui3-846", "yui3-978", "yui3-291", "yui3-1125", "yui3-1258", "yui3-715", "yui3-847", "yui3-979", "yui3-292", "yui3-1126", "yui3-1259", "yui3-716", "yui3-848", "yui3-293", "yui3-1127", "yui3-717", "yui3-849", "yui3-294", "yui3-1390", "yui3-1128", "yui3-980", "yui3-718", "yui3-295", "yui3-1391", "yui3-1129", "yui3-981", "yui3-719", "yui3-296", "yui3-1260", "yui3-1392", "yui3-850", "yui3-982", "yui3-297", "yui3-1261", "yui3-1393", "yui3-851", "yui3-983", "yui3-1130", "yui3-298", "yui3-1262", "yui3-1394", "yui3-720", "yui3-852", "yui3-984", "yui3-1131", "yui3-299", "yui3-1263", "yui3-1395", "yui3-721", "yui3-853", "yui3-985", "yui3-1000", "yui3-1132", "yui3-1264", "yui3-1396", "yui3-722", "yui3-854", "yui3-986", "yui3-1001", "yui3-1133", "yui3-1265", "yui3-1397", "yui3-855", "yui3-987", "yui3-1002", "yui3-1134", "yui3-1266", "yui3-1398", "yui3-723", "yui3-856", "yui3-988", "yui3-1003", "yui3-1135", "yui3-1267", "yui3-1399", "yui3-724", "yui3-857", "yui3-989", "yui3-1004", "yui3-1136", "yui3-1268", "yui3-725", "yui3-858", "yui3-1005", "yui3-1137", "yui3-1269", "yui3-726", "yui3-859", "yui3-1006", "yui3-1138", "yui3-727", "yui3-1007", "yui3-1139", "yui3-990", "yui3-728", "yui3-1270", "yui3-1008", "yui3-991", "yui3-729", "yui3-1271", "yui3-1009", "yui3-860", "yui3-992", "yui3-1140", "yui3-1272", "yui3-861", "yui3-993", "yui3-1141", "yui3-1273", "yui3-730", "yui3-862", "yui3-994", "yui3-1010", "yui3-1142", "yui3-1274", "yui3-731", "yui3-863", "yui3-995", "yui3-1011", "yui3-1143", "yui3-1275", "yui3-600", "yui3-732", "yui3-864", "yui3-996", "yui3-1012", "yui3-1144", "yui3-1276", "yui3-601", "yui3-733", "yui3-865", "yui3-997", "yui3-1277", "yui3-1145", "yui3-1013", "yui3-602", "yui3-734", "yui3-866", "yui3-998", "yui3-1278", "yui3-1146", "yui3-1014", "yui3-603", "yui3-735", "yui3-867", "yui3-999", "yui3-1279", "yui3-1147", "yui3-1015", "yui3-604", "yui3-736", "yui3-868", "yui3-1148", "yui3-1016", "yui3-605", "yui3-737", "yui3-869", "yui3-1149", "yui3-1017", "yui3-606", "yui3-738", "yui3-1280", "yui3-1018", "yui3-607", "yui3-739", "yui3-1281", "yui3-1019", "yui3-608", "yui3-870", "yui3-1282", "yui3-1150", "yui3-609", "yui3-871", "yui3-1283", "yui3-1151", "yui3-740", "yui3-872", "yui3-1284", "yui3-1152", "yui3-1020", "yui3-741", "yui3-873", "yui3-1285", "yui3-1153", "yui3-1021", "yui3-610", "yui3-742", "yui3-874", "yui3-1286", "yui3-1154", "yui3-1022", "yui3-611", "yui3-743", "yui3-875", "yui3-1287", "yui3-1155", "yui3-1023", "yui3-612", "yui3-744", "yui3-876", "yui3-1288", "yui3-1156", "yui3-1024", "yui3-613", "yui3-745", "yui3-877", "yui3-1289", "yui3-1157", "yui3-1025", "yui3-614", "yui3-746", "yui3-878", "yui3-1158", "yui3-1026", "yui3-615", "yui3-747", "yui3-879", "yui3-1159", "yui3-1027", "yui3-1290", "yui3-616", "yui3-748", "yui3-1028", "yui3-617", "yui3-749", "yui3-1029", "yui3-1291", "yui3-618", "yui3-880", "yui3-1292", "yui3-1160", "yui3-619", "yui3-881", "yui3-1293", "yui3-1161", "yui3-750", "yui3-882", "yui3-1294", "yui3-1162", "yui3-1030", "yui3-751", "yui3-883", "yui3-1295", "yui3-1163", "yui3-1031", "yui3-620", "yui3-752", "yui3-884", "yui3-1296", "yui3-1164", "yui3-1032", "yui3-621", "yui3-753", "yui3-885", "yui3-1297", "yui3-1165", "yui3-1033", "yui3-622", "yui3-754", "yui3-886", "yui3-1298", "yui3-1166", "yui3-1034", "yui3-623", "yui3-755", "yui3-887", "yui3-1299", "yui3-1167", "yui3-624", "yui3-756", "yui3-888", "yui3-1168", "yui3-625", "yui3-757", "yui3-889", "yui3-1169", "yui3-626", "yui3-758", "yui3-627", "yui3-759", "yui3-628", "yui3-890", "yui3-1170", "yui3-629", "yui3-891", "yui3-1171", "yui3-760", "yui3-892", "yui3-1172", "yui3-1040", "yui3-761", "yui3-893", "yui3-1173", "yui3-1041", "yui3-630", "yui3-762", "yui3-894", "yui3-1174", "yui3-1042", "yui3-631", "yui3-763", "yui3-895", "yui3-1175", "yui3-1043", "yui3-500", "yui3-632", "yui3-764", "yui3-896", "yui3-1176", "yui3-1044", "yui3-501", "yui3-633", "yui3-765", "yui3-897", "yui3-1177", "yui3-1045", "yui3-502", "yui3-634", "yui3-766", "yui3-898", "yui3-1178", "yui3-1046", "yui3-503", "yui3-635", "yui3-767", "yui3-899", "yui3-1179", "yui3-1047", "yui3-504", "yui3-636", "yui3-768", "yui3-1048", "yui3-505", "yui3-637", "yui3-769", "yui3-1049", "yui3-1180", "yui3-506", "yui3-638", "yui3-507", "yui3-639", "yui3-1181", "yui3-508", "yui3-770", "yui3-1182", "yui3-1050", "yui3-509", "yui3-771", "yui3-1183", "yui3-1051", "yui3-640", "yui3-772", "yui3-1184", "yui3-1052", "yui3-641", "yui3-773", "yui3-1185", "yui3-1053", "yui3-510", "yui3-642", "yui3-774", "yui3-1186", "yui3-1054", "yui3-511", "yui3-643", "yui3-775", "yui3-1187", "yui3-1055", "yui3-512", "yui3-644", "yui3-776", "yui3-1188", "yui3-1056", "yui3-513", "yui3-645", "yui3-777", "yui3-1189", "yui3-1057", "yui3-514", "yui3-646", "yui3-778", "yui3-1058", "yui3-515", "yui3-647", "yui3-779", "yui3-1059", "yui3-1190", "yui3-516", "yui3-648", "yui3-1191", "yui3-517", "yui3-649", "yui3-518", "yui3-780", "yui3-1192", "yui3-1060", "yui3-519", "yui3-781", "yui3-1193", "yui3-1061", "yui3-650", "yui3-782", "yui3-1194", "yui3-1062", "yui3-651", "yui3-783", "yui3-1195", "yui3-1063", "yui3-520", "yui3-652", "yui3-784", "yui3-1196", "yui3-1064", "yui3-521", "yui3-653", "yui3-785", "yui3-1197", "yui3-1065", "yui3-522", "yui3-654", "yui3-786", "yui3-1198", "yui3-1066", "yui3-523", "yui3-655", "yui3-787", "yui3-1199", "yui3-1067", "yui3-524", "yui3-656", "yui3-788", "yui3-1068", "yui3-525", "yui3-657", "yui3-789", "yui3-1069", "yui3-526", "yui3-658", "yui3-527", "yui3-659", "yui3-1070", "yui3-528", "yui3-790", "3.0.0beta1m1", "yui3-529", "yui3-791", "yui3-1071", "3.0.0beta1m2", "yui3-660", "yui3-792", "yui3-1072", "3.0.0beta1m3", "yui3-661", "yui3-793", "yui3-1073", "yui3-530", "yui3-662", "yui3-794", "yui3-1074", "yui3-531", "yui3-663", "yui3-795", "yui3-400", "yui3-1075", "yui3-532", "yui3-664", "yui3-796", "yui3-401", "yui3-1076", "yui3-533", "yui3-665", "yui3-797", "yui3-402", "yui3-1077", "yui3-534", "yui3-666", "yui3-798", "yui3-403", "yui3-1078", "yui3-535", "yui3-667", "yui3-799", "yui3-1500", "yui3-404", "yui3-1079", "yui3-536", "yui3-668", "yui3-1501", "yui3-405", "yui3-537", "yui3-669", "yui3-1502", "yui3-406", "yui3-1080", "yui3-538", "yui3-1503", "yui3-407", "yui3-1081", "yui3-539", "yui3-1504", "yui3-408", "yui3-670", "yui3-1505", "yui3-409", "yui3-1082", "yui3-671", "yui3-1506", "yui3-1083", "yui3-540", "yui3-672", "yui3-1507", "yui3-1084", "yui3-541", "yui3-673", "yui3-1508", "yui3-410", "yui3-1085", "yui3-542", "yui3-674", "yui3-1509", "yui3-411", "yui3-1086", "yui3-543", "yui3-675", "yui3-412", "yui3-1087", "yui3-544", "yui3-676", "yui3-413", "yui3-1088", "yui3-545", "yui3-677", "yui3-1510", "yui3-414", "yui3-1089", "yui3-546", "yui3-678", "yui3-1511", "yui3-415", "yui3-547", "yui3-679", "yui3-1090", "yui3-1512", "yui3-416", "yui3-548", "yui3-1091", "yui3-1513", "yui3-417", "yui3-549", "yui3-1092", "yui3-1514", "yui3-418", "yui3-680", "yui3-1515", "3.0.0b1", "yui3-419", "yui3-681", "yui3-1093", "yui3-1516", "yui3-550", "yui3-682", "yui3-1094", "yui3-1517", "yui3-551", "yui3-683", "yui3-1095", "yui3-1518", "yui3-420", "yui3-552", "yui3-684", "yui3-1096", "yui3-1519", "yui3-421", "yui3-553", "yui3-685", "yui3-1097", "yui3-422", "yui3-554", "yui3-686", "yui3-1098", "yui3-423", "yui3-555", "yui3-687", "yui3-1099", "yui3-1520", "yui3-424", "yui3-556", "yui3-688", "yui3-1521", "yui3-425", "yui3-557", "yui3-689", "yui3-1522", "yui3-426", "yui3-558", "yui3-1523", "yui3-427", "yui3-559", "yui3-1524", "yui3-428", "yui3-690", "yui3-1525", "yui3-429", "yui3-691", "yui3-1526", "yui3-560", "yui3-692", "yui3-1527", "yui3-561", "yui3-693", "yui3-430", "yui3-1528", "yui3-562", "yui3-694", "yui3-431", "yui3-1529", "yui3-563", "yui3-695", "yui3-432", "yui3-300", "yui3-564", "yui3-696", "yui3-433", "yui3-301", "yui3-565", "yui3-697", "yui3-1530", "yui3-434", "yui3-302", "yui3-566", "yui3-698", "yui3-1531", "yui3-435", "yui3-303", "yui3-567", "yui3-699", "yui3-1532", "yui3-1400", "yui3-436", "yui3-304", "yui3-568", "yui3-1533", "yui3-1401", "yui3-437", "yui3-305", "yui3-569", "yui3-1534", "yui3-1402", "yui3-438", "yui3-306", "yui3-1535", "yui3-1403", "yui3-439", "yui3-307", "yui3-1536", "yui3-1404", "yui3-308", "yui3-570" };


void
test_array_length(void **state)
{
    assert(ARRAY_LENGH(unsorted) == 9);
}

void
test_sort(void **state)
{
    version_sorter_sort(unsorted, ARRAY_LENGH(unsorted));
    int i;
    for (i = 0; i < ARRAY_LENGH(unsorted); i++) {
        assert(strcmp(unsorted[i], expected_sorted[i]) == 0);
    }
}

void
test_parse_version_word(void **state)
{
    StringLinkedList *sll = string_linked_list_init();

    parse_version_word("1.0.10a", sll);
    
    assert((strcmp("1", sll->head->str)) == 0);
    assert((strcmp("0", sll->head->next->str)) == 0);
    assert((strcmp("10", sll->head->next->next->str)) == 0);
    assert((strcmp("a", sll->tail->str)) == 0);
    
    string_linked_list_free(sll);
}

void
test_version_flatten(void **state)
{
    StringLinkedList *sll = string_linked_list_init();
    
    parse_version_word("1.0.10a", sll);
    char *flat = create_flattened_version(sll, 2);
    
    assert((strcmp(" 1 010 a", flat)) == 0);
    
    free(flat);
    string_linked_list_free(sll);
}

static void 
benchmark_sort(void **state)
{
    int i;
    clock_t real_start, real_end;
    struct tms start, end;
    double user, system, real;
    
    real_start = times(&start);
    for (i = 0; i < 10; i++) {
        version_sorter_sort(benchmark_list, ARRAY_LENGH(benchmark_list));
    }
    real_end = times(&end);
    
    real = ((double)(real_end-real_start)) / sysconf(_SC_CLK_TCK);
    user = ((double)(end.tms_utime-start.tms_utime)) / sysconf(_SC_CLK_TCK);
    system = ((double)(end.tms_stime-start.tms_stime)) / sysconf(_SC_CLK_TCK);
    printf("Real Time: %f\nUser Time: %f\nSystem Time: %f\n", real, user, system);
}

int
main(void)
{    
    const UnitTest tests[] = {
        unit_test(test_hashtable),
        unit_test(test_array_length),
        unit_test(test_sort),
        unit_test(test_parse_version_word),
        unit_test(test_version_flatten),
        unit_test(benchmark_sort),
    };
    return run_tests(tests);
}