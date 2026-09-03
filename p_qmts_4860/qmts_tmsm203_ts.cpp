/************************/
/*** 2026-03-13 ********/
/*** 设备周期维护——按USER_ID合并推送 ***/
/************************/

#include "stdafx.h"
#include "epex.h"
#include <map>
#include <string>
#include <vector>

/* ***** 外部函数申明 ***** */
int f_push_baowu_chat(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

// 按 USER_ID 存储消息统计
struct UserPushInfo
{
	// 临期 - 点检员
	std::map<CString, int> respMap;

	// 作业长：临期、过期
	std::map<CString, int> areaAdventMap;  // 临期
	std::map<CString, int> areaAbnormalMap;// 过期

	// 工场长：临期、过期 
	std::map<CString, int> factoryAdventMap;  // 临期
	std::map<CString, int> factoryAbnormalMap; // 过期

	// 过期 - 室主任（按作业区统计所有过期）
	std::map<CString, int> directorAreaMap;
};

typedef std::map<CString, UserPushInfo> UserMessageMap;

BM2F_ENTERACE(qmts_tmsm203_ts)
BM2_FUNCTION_EXPORT
int f_qmts_tmsm203_ts(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	int fetchRowCount = 0;
	CString strSql;
	CString strNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString strToday = CDateTime::Now().ToString("yyyyMMdd");
	CString tap_date = strToday;

	Log::Trace("", "", "tap_date{0}", tap_date);

	EIClass bcls_rec_s;
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "TAP_DATE");
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "CONTENT");
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "TOPIC");
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "CODE");
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "USER_ID");

	// 推送配置
	CString strModuleName = " ";
	CString strSenderId = " ";
	CString strMsgTitle = " ";
	CString strMsgTemplate = " ";
	CString strPushSwitch = " ";

	UserMessageMap mapUserMsg;

	try
	{
		// 查询推送配置
		strSql = "SELECT MODULE_NAME, LOGINNAME, MESSAGE_TITLE, MESSAGE_LIST, KZABSCHL FROM TQMTSBWU WHERE CODE = '26' ";

		CDbCommand cmdConfig(conn);
		cmdConfig.SetCommandText(strSql);
		cmdConfig.ExecuteReader();

		if (cmdConfig.Read())
		{
			strModuleName = cmdConfig.GetString(1);
			strSenderId = cmdConfig.GetString(2);
			strMsgTitle = cmdConfig.GetString(3);
			strMsgTemplate = cmdConfig.GetString(4);
			strPushSwitch = cmdConfig.GetString(5);
		}
		cmdConfig.Close();

		if (strPushSwitch == "1")
		{
			// 查询临期/超期设备
			strSql = "SELECT TT.SEQ_NO, TT.WORK_AREA, TT.DEV_NAME, TT.ITEM_NAME, TT.RESP, TT.IS_ABNORMAL, TT.IS_ADVENT, TT.SHIFT_CLASS ";
			strSql += " FROM (SELECT t.SEQ_NO, t.WORK_AREA, t.DEV_NAME, t.ITEM_NAME, t.RESP, t.CHECK_FLAG, t.PERD_DD1, t.SHIFT_CLASS,";
			strSql += " CASE WHEN ((TRUNC(SYSDATE) - TO_DATE(TRIM(t.TIME_2),'yyyymmdd')) > 0 AND t.CHECK_FLAG = '0') THEN '1' ELSE '0' END AS IS_ABNORMAL, ";
			strSql += " CASE WHEN ((TO_DATE(TRIM(t.TIME_2),'yyyymmdd') -TRUNC(SYSDATE)) < t.PERD_DD1 AND t.CHECK_FLAG = '0') THEN '1' ELSE '0' END AS IS_ADVENT ";
			strSql += " FROM TTMSM203 t WHERE t.BACKC1=' ') TT WHERE (TT.IS_ADVENT = '1' or TT.IS_ABNORMAL = '1') ORDER BY TT.SEQ_NO DESC";

			CDbCommand cmdMain(conn);
			cmdMain.SetCommandText(strSql);
			cmdMain.ExecuteReader();

			mapUserMsg.clear();
			fetchRowCount = 0;
			while (cmdMain.Read())
			{
				fetchRowCount++;
				CString workArea = cmdMain.GetString(2).Trim();
				CString resp = cmdMain.GetString(5).Trim();
				CString isAbnormal = cmdMain.GetString(6);
				CString isAdvent = cmdMain.GetString(7);
				CString shiftno = cmdMain.GetString(8).Trim();
				CString areaPrefix = workArea.SubstringNE(0, 2);
				// ====================== 临期设备处理 ======================
				if (isAdvent == "1" && isAbnormal == "0")
				{
					// 规则1：临期 → 点检员
					if (!resp.IsEmpty())
					{
						CString sqlUser = "SELECT USER_ID FROM TQMTSBWU_YH WHERE USER_NAME = '" + resp + "' AND GWNAME='点检员' AND CODE = '26'";
						CDbCommand cmdUser(conn);
						cmdUser.SetCommandText(sqlUser);
						cmdUser.ExecuteReader();
						while (cmdUser.Read())
						{
							CString userId = cmdUser.GetString(1).Trim();
							if (!userId.IsEmpty())
							{
								mapUserMsg[userId].respMap[resp]++;
							}
						}
						cmdUser.Close();
					}

					// 规则2：临期 → 作业长
					if (!workArea.IsEmpty())
					{
						CString sqlArea;
						CString workAreaN;
						if (workArea == "行车保障作业区")
						{
							sqlArea = "SELECT USER_ID FROM TQMTSBWU_YH WHERE WORK_AREA= '" + workArea + "' AND GWNAME='作业长' AND REMARK LIKE '" + shiftno.SubstringNE(0, 2) + "%' AND CODE = '26'";
							workAreaN = "行车保障作业区--" + shiftno.SubstringNE(0, 2);
						}
						else
						{
							sqlArea = "SELECT USER_ID FROM TQMTSBWU_YH WHERE WORK_AREA= '" + workArea + "' AND GWNAME='作业长' AND CODE = '26'";
							workAreaN = workArea;
						}
						
						CDbCommand cmdArea(conn);
						cmdArea.SetCommandText(sqlArea);
						cmdArea.ExecuteReader();
						while (cmdArea.Read())
						{
							CString userId = cmdArea.GetString(1).Trim();
							if (!userId.IsEmpty())
							{
								mapUserMsg[userId].areaAdventMap[workAreaN]++;
							}
						}
						cmdArea.Close();
					}
					// 3. 临期 → 工场长
					if (!areaPrefix.IsEmpty())
					{
						CString sqlFac = "SELECT USER_ID FROM TQMTSBWU_YH WHERE SUBSTR(WORK_AREA,0,1)='" + areaPrefix + "' AND GWNAME='工场长' AND CODE='26'";
						CDbCommand cmdFac(conn);
						cmdFac.SetCommandText(sqlFac);
						cmdFac.ExecuteReader();
						while (cmdFac.Read())
						{
							CString userId = cmdFac.GetString(1).Trim();
							if (!userId.IsEmpty())
								mapUserMsg[userId].factoryAdventMap[areaPrefix]++;
						}
						cmdFac.Close();
					}
				}

				// ====================== 过期设备处理 ======================
				if (isAbnormal == "1")
				{
					// 过期 → 作业长
					if (!workArea.IsEmpty())
					{
						CString sqlArea;
						CString workAreaN;
						if (workArea == "行车保障作业区")
						{
							sqlArea = "SELECT USER_ID FROM TQMTSBWU_YH WHERE WORK_AREA= '" + workArea + "' AND GWNAME='作业长' AND REMARK LIKE '" + shiftno.SubstringNE(0, 2) + "%' AND CODE = '26'";
							workAreaN = "行车保障作业区--" + shiftno.SubstringNE(0, 2);
						}
						else
						{
							sqlArea = "SELECT USER_ID FROM TQMTSBWU_YH WHERE WORK_AREA= '" + workArea + "' AND GWNAME='作业长' AND CODE = '26'";
							workAreaN = workArea;
						}
						CDbCommand cmdArea(conn);
						cmdArea.SetCommandText(sqlArea);
						cmdArea.ExecuteReader();
						while (cmdArea.Read())
						{
							CString userId = cmdArea.GetString(1).Trim();
							if (!userId.IsEmpty())
								mapUserMsg[userId].areaAbnormalMap[workAreaN]++;
						}
						cmdArea.Close();
					}
					// 规则：过期 → 工场长
					if (!areaPrefix.IsEmpty())
					{
						CString sqlFactory = "SELECT USER_ID FROM TQMTSBWU_YH WHERE SUBSTR(WORK_AREA,0,1) = '" + areaPrefix + "' AND GWNAME='工场长' AND CODE = '26'";
						CDbCommand cmdFactory(conn);
						cmdFactory.SetCommandText(sqlFactory);
						cmdFactory.ExecuteReader();
						while (cmdFactory.Read())
						{
							CString userId = cmdFactory.GetString(1).Trim();
							if (!userId.IsEmpty())
							{
								mapUserMsg[userId].factoryAbnormalMap[areaPrefix]++;
							}
						}
						cmdFactory.Close();
					}

					// 规则：过期 → 设备室主任
					CString sqlDirector = "SELECT USER_ID FROM TQMTSBWU_YH WHERE CODE = '26' AND (GWNAME='主任' OR GWNAME='管理')";
					CDbCommand cmdDirector(conn);
					cmdDirector.SetCommandText(sqlDirector);
					cmdDirector.ExecuteReader();
					while (cmdDirector.Read())
					{
						CString userId = cmdDirector.GetString(1).Trim();
						if (!userId.IsEmpty())
						{
							mapUserMsg[userId].directorAreaMap[workArea]++;
						}
					}
					cmdDirector.Close();
				}
			}
			cmdMain.Close();

			// ====================== 遍历推送 ======================
			for (auto& pair : mapUserMsg)
			{
				CString userId = pair.first;
				UserPushInfo& info = pair.second;

				CString pushContent;
				bool hasMessage = false;

				// 1. 点检员
				for (auto& respPair : info.respMap)
				{
					CString name = respPair.first;
					int cnt = respPair.second;
					CString cntStr;
					cntStr = std::to_string(cnt).c_str();
					CString tmp = name + "，您有" + cntStr + "条临期设备需维护";
					if (!pushContent.IsEmpty())
						pushContent += "\r\n";
					pushContent += tmp;
					hasMessage = true;
				}
				Log::Trace("", "", "{0}", pushContent);
				// 2. 作业长
				std::map<CString, std::pair<int, int>> areaStat;
				for (auto& p : info.areaAdventMap) areaStat[p.first].first = p.second;
				for (auto& p : info.areaAbnormalMap) areaStat[p.first].second = p.second;

				for (auto& p : areaStat)
				{
					CString a, e;
					int cnt1 = p.second.first;
					int cnt2 = p.second.second;
					a = std::to_string(cnt1).c_str();
					e = std::to_string(cnt2).c_str();
					CString tmp = p.first + "：临期" + a + "条，过期" + e + "条设备需维护";
					pushContent += tmp + "\r\n";
					hasMessage = true;
				}
				// 3. 工场长：临期 + 过期 分开显示
				std::map<CString, std::pair<int, int>> facStat;
				for (auto& p : info.factoryAdventMap) facStat[p.first].first = p.second;
				for (auto& p : info.factoryAbnormalMap) facStat[p.first].second = p.second;

				for (auto& p : facStat)
				{
					CString name = p.first;
					if (name == "南") name = "南区";
					else if (name == "北") name = "北区";
					else if (name == "特") name = "特钢";
					else if (name == "行") name = "行车";

					CString a, e;
					int cnt1 = p.second.first;
					int cnt2 = p.second.second;
					a = std::to_string(cnt1).c_str();
					e = std::to_string(cnt2).c_str();
					
					CString tmp = name + "工场：临期" + a + "条，过期" + e + "条设备需维护";
					pushContent += tmp + "\r\n";
					hasMessage = true;
				}
				
				

				// 4. 室主任
				if (!info.directorAreaMap.empty())
				{
					if (!pushContent.IsEmpty())
						pushContent += "\r\n";
					pushContent += "【各作业区过期设备统计】\r\n";
					for (auto& dirPair : info.directorAreaMap)
					{
						CString area = dirPair.first;
						int cnt = dirPair.second;
						CString cntStr;
						cntStr = std::to_string(cnt).c_str();
						CString tmp = area + "：" + cntStr + "条过期设备需维护";
						pushContent += tmp + "\r\n";
					}
					hasMessage = true;
				}

				if (!hasMessage)
					continue;

				// 推送
				bcls_rec_s.Tables[0].Rows.Clear();
				bcls_rec_s.Tables[0].Rows.Add();
				bcls_rec_s.Tables[0].Rows[0]["TAP_DATE"] = tap_date;
				bcls_rec_s.Tables[0].Rows[0]["TOPIC"] = "设备周期维护提醒";
				bcls_rec_s.Tables[0].Rows[0]["CONTENT"] = pushContent;
				bcls_rec_s.Tables[0].Rows[0]["CODE"] = "26";
				bcls_rec_s.Tables[0].Rows[0]["USER_ID"] = userId;
				//bcls_rec_s.Tables[0].Rows[0]["USER_ID"] ="HM1009";

				doFlag = f_push_baowu_chat(&bcls_rec_s, bcls_ret, conn);
				if (doFlag < 0)
				{
					strcpy(s.msg, "推送接口调用失败");
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
		}

	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006"), arguments, 1);
		CString str = strSql + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}