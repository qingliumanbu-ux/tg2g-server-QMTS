//Dec 16 08:33//请不要修改此行

#include "stdafx.h"

BM2F_ENTERACE(qmtism_inq);

int f_qmtism_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	//系统日志类定义
	CTracer log(__FUNCTION__);

	//程序用变量
	int  doFlag = 0;
	CString q_rec_prod_time_from = " ";
	CString q_rec_prod_time_to = " ";
	CDbCommand sel_1(conn);
	//CDbCommand sel_2(conn);
	CString form_type = " ";
	try
	{
		q_rec_prod_time_from = bcls_rec->Tables[0].Rows[0]["PRE_PROD_TIME_FROM"].ToString().Trim();
		q_rec_prod_time_to = bcls_rec->Tables[0].Rows[0]["PRE_PROD_TIME_TO"].ToString().Trim();
		form_type = bcls_rec->Tables[1].Rows[0]["FORM_TYPE"].ToString().Trim();
	/*	CString sql_time = "";
		if (q_rec_prod_time_from!=NULL){
			sql_time += "AND VMMSM01.PROD_TIME >= " + q_rec_prod_time_from + " ";
		}
		if (q_rec_prod_time_to!=NULL){
			sql_time += "AND VMMSM01.PROD_TIME <= " + q_rec_prod_time_to + " ";
		}
		else
		{
			sql_time += "AND VMMSM01.PROD_TIME <= '99999999' ";
		}
		Log::Trace("", "", "[{0}]", sql_time);*/
		/*添加返回表的列*/
		if (form_type=="SM"){
			bcls_ret->Tables[0].set_TableName("QMTISM_INQ");
		}
		else if (form_type == "SN")
		{
			bcls_ret->Tables[0].set_TableName("QMTI01");
		}
		else
		{
			return doFlag;
		}
		bcls_ret->Tables[0].Columns.Add(DT_STRING,"ITEM_NAME_01");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "ITEM_NAME_02");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "ITEM_NAME_03");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "ITEM_NAME_04");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "ITEM_NAME_05");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "ITEM_NAME_06");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "ITEM_NAME_07");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "ITEM_NAME_08");
		int i = 0;
		/*
			以下查询需要执行3n次sql语句太耗时了
		*/
		//CString sql_1 = "SELECT SG_SIGN,SUM(MAT_WT) AS MAT_WT_SUM "
		//				"FROM VMMSM01,TQMTS23 "
		//				"WHERE VMMSM01.HEAT_NO=TQMTS23.HEAT_NO "
		//				"AND VMMSM01.PROD_TIME >= @rec_prod_time_from || ' ' "
		//				"AND VMMSM01.PROD_TIME <= @rec_prod_time_to || '999999' "
		//				"AND (VMMSM01.SURFACE_DECIDE_CODE='1' OR  VMMSM01.SURFACE_DECIDE_CODE='2') "
		//				"AND (TQMTS23.JUDGE_CODE='1' OR TQMTS23.JUDGE_CODE='2') "
		//				"GROUP BY VMMSM01.SG_SIGN";
		//Log::Trace("","","[{0}]",sql_1);
		//sel_1.SetCommandText(sql_1);
		//sel_1.Parameters.Set("rec_prod_time_from",q_rec_prod_time_from);
		//sel_1.Parameters.Set("rec_prod_time_to", q_rec_prod_time_to);
		//Log::Trace("", "","分组查询开始");
		//sel_1.ExecuteReader();
		//while (sel_1.Read()){
		//	/*生产厂*/
		//	CString factory = "SM";
		//	/*牌号（钢级）*/
		//	CString sg_sign = sel_1.GetString(1);
		//	/*检验量*/
		//	CDecimal mat_wt_sum = 0;
		//	if (sel_1.GetDecimal(2)!=NULL){
		//		mat_wt_sum = sel_1.GetDecimal(2);
		//	}
		//	Log::Trace("","","[{0}:{1}]",i,mat_wt_sum);
		//	/*总合格率*/
		//	CDecimal fpy = 0;
		//	/*成分合格率*/
		//	CDecimal judge_fpy = 0;
		//	/*物性合格率*/
		//	CDecimal phy_fpy = 0;
		//	/*外观合格率*/
		//	CDecimal face_fpy = 0;
		//	/*成分合格量*/
		//	CDecimal mat_wt_judge_sum = 0;
		//	/*外观合格量*/
		//	CDecimal mat_wt_face_sum = 0;
		//	/*不合格量*/
		//	CDecimal mat_wt_error_sum = 0;
		//	/*查询成分合格量*/
		//	CString sql_2 = "SELECT SUM(MAT_WT) AS MAT_WT_JUDGE_SUM "
		//					"FROM VMMSM01,TQMTS23 "
		//					"WHERE VMMSM01.HEAT_NO=TQMTS23.HEAT_NO "
		//					"AND VMMSM01.PROD_TIME >= @rec_prod_time_from || ' ' "
		//					"AND VMMSM01.PROD_TIME <= @rec_prod_time_to || '999999' "
		//					"AND VMMSM01.SG_SIGN ='" + sg_sign + "' "
		//					"AND TQMTS23.JUDGE_CODE='1' "
		//					"AND (VMMSM01.SURFACE_DECIDE_CODE='1' OR  VMMSM01.SURFACE_DECIDE_CODE='2') ";
		//	sel_2.SetCommandText(sql_2);
		//	sel_2.Parameters.Set("rec_prod_time_from", q_rec_prod_time_from);
		//	sel_2.Parameters.Set("rec_prod_time_to", q_rec_prod_time_to);
		//	sel_2.ExecuteReader();
		//	if (sel_2.Read()){
		//		if (sel_2.GetDecimal(1)!=NULL){
		//			mat_wt_judge_sum = sel_2.GetDecimal(1);
		//		}
		//		Log::Trace("","","judge[{0}]",mat_wt_judge_sum);
		//	}
		//	sel_2.Close();
		//	/*查询外观合格量*/
		//	CString sql_3 = "SELECT SUM(MAT_WT) AS MAT_WT_FACE_SUM "
		//					"FROM VMMSM01,TQMTS23 "
		//					"WHERE VMMSM01.HEAT_NO=TQMTS23.HEAT_NO "
		//					"AND VMMSM01.PROD_TIME >= @rec_prod_time_from || ' ' "
		//					"AND VMMSM01.PROD_TIME <= @rec_prod_time_to || '999999' "
		//					"AND VMMSM01.SG_SIGN = '" + sg_sign + "' "
		//					"AND VMMSM01.SURFACE_DECIDE_CODE='1' "
		//					"AND (TQMTS23.JUDGE_CODE='1' OR TQMTS23.JUDGE_CODE='2') ";
		//	sel_2.SetCommandText(sql_3);
		//	sel_2.Parameters.Set("rec_prod_time_from", q_rec_prod_time_from);
		//	sel_2.Parameters.Set("rec_prod_time_to", q_rec_prod_time_to);
		//	sel_2.ExecuteReader();
		//	if (sel_2.Read()){
		//		if (sel_2.GetDecimal(1)!=NULL){
		//			mat_wt_face_sum = sel_2.GetDecimal(1);
		//		}
		//	}
		//	sel_2.Close();
		//	/*查询不合格量*/
		//	CString sql_4 = "SELECT SUM(MAT_WT) AS MAT_WT_ERROR_SUM "
		//					"FROM VMMSM01,TQMTS23 "
		//					"WHERE VMMSM01.HEAT_NO=TQMTS23.HEAT_NO "
		//					"AND VMMSM01.PROD_TIME >= @rec_prod_time_from || ' ' "
		//					"AND VMMSM01.PROD_TIME <= @rec_prod_time_to || '999999' "
		//					"AND VMMSM01.SG_SIGN = '"+sg_sign+"' "
		//					"AND (TQMTS23.JUDGE_CODE='2' OR VMMSM01.SURFACE_DECIDE_CODE='2') ";
		//	sel_2.SetCommandText(sql_4);
		//	sel_2.Parameters.Set("rec_prod_time_from", q_rec_prod_time_from);
		//	sel_2.Parameters.Set("rec_prod_time_to", q_rec_prod_time_to);
		//	sel_2.ExecuteReader();
		//	if (sel_2.Read()){
		//		if (sel_2.GetDecimal(1)!=NULL){
		//			mat_wt_error_sum = sel_2.GetDecimal(1);
		//		}
		//	}
		//	sel_2.Close();
		CString sql_sm =  "SELECT sjf.*,e.MAT_WT_ERROR_SUM FROM "
						    "(SELECT sj.*, MAT_WT_FACE_SUM FROM "
							"(SELECT s.SG_SIGN, s.MAT_WT_SUM, j.MAT_WT_JUDGE_SUM FROM "
							"(SELECT VMMSM01.SG_SIGN, SUM(MAT_WT) AS MAT_WT_SUM "
							"FROM VMMSM01, TQMTS23 "
							"WHERE VMMSM01.HEAT_NO = TQMTS23.HEAT_NO "
							"AND VMMSM01.PROD_TIME >= @rec_prod_time_from || ' ' "
							"AND VMMSM01.PROD_TIME <= @rec_prod_time_to || '999999' "
							"AND(VMMSM01.SURFACE_DECIDE_CODE = '1' OR  VMMSM01.SURFACE_DECIDE_CODE = '2') "
							"AND(TQMTS23.JUDGE_CODE = '1' OR TQMTS23.JUDGE_CODE = '2') "
							"GROUP BY VMMSM01.SG_SIGN) s LEFT JOIN "
							"(SELECT VMMSM01.SG_SIGN, SUM(MAT_WT) AS MAT_WT_JUDGE_SUM "
							"FROM VMMSM01, TQMTS23 "
							"WHERE VMMSM01.HEAT_NO = TQMTS23.HEAT_NO "
							"AND VMMSM01.PROD_TIME >= @rec_prod_time_from || ' ' "
							"AND VMMSM01.PROD_TIME <= @rec_prod_time_to || '999999' "
							"AND TQMTS23.JUDGE_CODE = '1' "
							"AND(VMMSM01.SURFACE_DECIDE_CODE = '1' OR  VMMSM01.SURFACE_DECIDE_CODE = '2') "
							"GROUP BY VMMSM01.SG_SIGN) j "
							"ON s.SG_SIGN = j.SG_SIGN) sj LEFT JOIN "
							"(SELECT VMMSM01.SG_SIGN, SUM(MAT_WT) AS MAT_WT_FACE_SUM "
							"FROM VMMSM01, TQMTS23 "
							"WHERE VMMSM01.HEAT_NO = TQMTS23.HEAT_NO "
							"AND VMMSM01.PROD_TIME >= @rec_prod_time_from || ' ' "
							"AND VMMSM01.PROD_TIME <= @rec_prod_time_to || '999999' "
							"AND VMMSM01.SURFACE_DECIDE_CODE = '1' "
							"AND(TQMTS23.JUDGE_CODE = '1' OR TQMTS23.JUDGE_CODE = '2') "
							"GROUP BY VMMSM01.SG_SIGN) f "
							"ON sj.SG_SIGN = f.SG_SIGN) sjf LEFT JOIN "
							"(SELECT VMMSM01.SG_SIGN, SUM(MAT_WT) AS MAT_WT_ERROR_SUM "
							"FROM VMMSM01, TQMTS23 "
							"WHERE VMMSM01.HEAT_NO = TQMTS23.HEAT_NO "
							"AND VMMSM01.PROD_TIME >= @rec_prod_time_from || ' ' "
							"AND VMMSM01.PROD_TIME <= @rec_prod_time_to || '999999' "
							"AND ((TQMTS23.JUDGE_CODE='2' AND (VMMSM01.SURFACE_DECIDE_CODE='1' OR  VMMSM01.SURFACE_DECIDE_CODE='2')) OR "
							"(VMMSM01.SURFACE_DECIDE_CODE = '2' AND TQMTS23.JUDGE_CODE = '1')) "
							"GROUP BY VMMSM01.SG_SIGN) e "
							"ON sjf.SG_SIGN = e.SG_SIGN";
		sel_1.SetCommandText(sql_sm);
		sel_1.Parameters.Set("rec_prod_time_from",q_rec_prod_time_from);
		sel_1.Parameters.Set("rec_prod_time_to", q_rec_prod_time_to);
		Log::Trace("", "","查询开始1");
		sel_1.ExecuteReader();
		Log::Trace("","","开始循环压值");
		while (sel_1.Read()){
			/*生产厂*/
			CString factory = "SM";
			/*牌号（钢级）*/
			CString sg_sign = sel_1.GetString(1);
			/*去掉牌号为空的记录*/
			if (sg_sign.IsEmpty()|| sg_sign == " "){
				continue;
			}
			/*检验量*/
			CDecimal mat_wt_sum = 0;
			if (sel_1.GetDecimal(2)!=NULL){
				mat_wt_sum = sel_1.GetDecimal(2);
			}
			/*总合格率*/
			CDecimal fpy = 0;
			/*成分合格率*/
			CDecimal judge_fpy = 0;
			/*物性合格率*/
			CDecimal phy_fpy = 0;
			/*外观合格率*/
			CDecimal face_fpy = 0;
			/*成分合格量*/
			CDecimal mat_wt_judge_sum = 0;
			if (sel_1.GetDecimal(3) != NULL){
				mat_wt_judge_sum = sel_1.GetDecimal(3);
			}
			/*外观合格量*/
			CDecimal mat_wt_face_sum = 0;
			if (sel_1.GetDecimal(4) != NULL){
				mat_wt_face_sum = sel_1.GetDecimal(4);
			}
			/*不合格量*/
			CDecimal mat_wt_error_sum = 0;
			if (sel_1.GetDecimal(5) != NULL){
				mat_wt_error_sum = sel_1.GetDecimal(5);
			}
			fpy = (mat_wt_sum - mat_wt_error_sum)*100 / mat_wt_sum;
			judge_fpy = mat_wt_judge_sum * 100 / mat_wt_sum;
			face_fpy = mat_wt_face_sum * 100 / mat_wt_sum;
			/*压入列值*/
			bcls_ret->Tables[0].Rows.Add();
			bcls_ret->Tables[0].Rows[i]["ITEM_NAME_01"] = factory;
			bcls_ret->Tables[0].Rows[i]["ITEM_NAME_02"] = sg_sign;
			bcls_ret->Tables[0].Rows[i]["ITEM_NAME_03"] = mat_wt_sum.Round(2).ToString();
			bcls_ret->Tables[0].Rows[i]["ITEM_NAME_04"] = fpy.Round(2).ToString();
			bcls_ret->Tables[0].Rows[i]["ITEM_NAME_05"] = judge_fpy.Round(2).ToString();
			bcls_ret->Tables[0].Rows[i]["ITEM_NAME_06"] = phy_fpy.Round(2).ToString();
			bcls_ret->Tables[0].Rows[i]["ITEM_NAME_07"] = face_fpy.Round(2).ToString();
			bcls_ret->Tables[0].Rows[i]["ITEM_NAME_08"] = mat_wt_error_sum.Round(2).ToString();
			i++;
		}
		sel_1.Close();
		/*查询VMMBW01，VMMCR01，VMMHP01，VMMHR01*/
		CString tables[] = {"VMMBW01","VMMCR01","VMMHP01","VMMHR01"};
		CString factorys[] = {"BW","CR","HP","HR"};
		CString sql_oth = "SELECT sfp.*,e.MAT_WT_ERROR_SUM FROM "
			"(SELECT sf.*, p.MAT_WT_PHY_SUM FROM "
			"(SELECT s.*, f.MAT_WT_FACE_SUM FROM "
			"(SELECT SG_SIGN, SUM(MAT_WT) AS MAT_WT_SUM "
			"FROM Table_Name "
			"WHERE "
			"PROD_TIME >= @rec_prod_time_from || ' ' "
			"AND PROD_TIME <= @rec_prod_time_to || '999999' "
			"AND(SURFACE_DECIDE_CODE = '1' OR SURFACE_DECIDE_CODE = '2') "
			"AND(PCH_JUDGE_CODE = '1' OR PCH_JUDGE_CODE = '2') "
			"GROUP BY SG_SIGN) s LEFT JOIN "
			"(SELECT SG_SIGN, SUM(MAT_WT) AS MAT_WT_FACE_SUM "
			"FROM Table_Name "
			"WHERE "
			"PROD_TIME >= @rec_prod_time_from || ' ' "
			"AND PROD_TIME <= @rec_prod_time_to || '999999' "
			"AND SURFACE_DECIDE_CODE = '1' "
			"AND(PCH_JUDGE_CODE = '1' OR PCH_JUDGE_CODE = '2') "
			"GROUP BY SG_SIGN) f "
			"ON s.SG_SIGN = f.SG_SIGN) sf LEFT JOIN "
			"(SELECT SG_SIGN, SUM(MAT_WT) AS MAT_WT_PHY_SUM "
			"FROM Table_Name "
			"WHERE "
			"PROD_TIME >= @rec_prod_time_from || ' ' "
			"AND PROD_TIME <= @rec_prod_time_to || '999999' "
			"AND(SURFACE_DECIDE_CODE = '1' OR SURFACE_DECIDE_CODE = '2') "
			"AND PCH_JUDGE_CODE = '1' "
			"GROUP BY SG_SIGN) p "
			"ON sf.SG_SIGN = p.SG_SIGN) sfp LEFT JOIN "
			"(SELECT SG_SIGN, SUM(MAT_WT) AS MAT_WT_ERROR_SUM "
			"FROM Table_Name "
			"WHERE "
			"PROD_TIME >= @rec_prod_time_from || ' ' "
			"AND PROD_TIME <= @rec_prod_time_to || '999999' "
			"AND((SURFACE_DECIDE_CODE = '2' AND(PCH_JUDGE_CODE = '1' OR PCH_JUDGE_CODE = '2')) "
			"OR(SURFACE_DECIDE_CODE = '1' AND PCH_JUDGE_CODE = '2')) "
			"GROUP BY SG_SIGN) e "
			"ON sfp.SG_SIGN = e.SG_SIGN";
		CString strOld = "Table_Name";
		Log::Trace("", "", "查询开始2");
		for (int j = 0;j<4;j++){
			CString strNew = tables[j];
			Log::Trace("","","strNew:[{0}]",strNew);
			CString sql_new = sql_oth.Replace(strOld, strNew);
			Log::Trace("","","[{0}]",sql_new);
			sel_1.SetCommandText(sql_new);
			sel_1.Parameters.Set("rec_prod_time_from", q_rec_prod_time_from);
			sel_1.Parameters.Set("rec_prod_time_to", q_rec_prod_time_to);
			sel_1.ExecuteReader();
			while (sel_1.Read()){
				/*生产厂*/
				CString factory = factorys[j];
				/*牌号（钢级）*/
				CString sg_sign = sel_1.GetString(1);
				/*去掉牌号为空的记录*/
				if (sg_sign.IsEmpty() || sg_sign == " "){
					continue;
				}
				/*检验量*/
				CDecimal mat_wt_sum = 0;
				if (sel_1.GetDecimal(2) != NULL){
					mat_wt_sum = sel_1.GetDecimal(2);
				}
				/*总合格率*/
				CDecimal fpy = 0;
				/*成分合格率*/
				CDecimal judge_fpy = 0;
				/*物性合格率*/
				CDecimal phy_fpy = 0;
				/*外观合格率*/
				CDecimal face_fpy = 0;
				/*外观合格量*/
				CDecimal mat_wt_face_sum = 0;
				if (sel_1.GetDecimal(3) != NULL){
					mat_wt_face_sum = sel_1.GetDecimal(3);
				}
				/*物性合格量*/
				CDecimal mat_wt_phy_sum = 0;
				if (sel_1.GetDecimal(4) != NULL){
					mat_wt_phy_sum = sel_1.GetDecimal(4);
				}
				/*不合格量*/
				CDecimal mat_wt_error_sum = 0;
				if (sel_1.GetDecimal(5) != NULL){
					mat_wt_error_sum = sel_1.GetDecimal(5);
				}
				fpy = (mat_wt_sum - mat_wt_error_sum) * 100 / mat_wt_sum;
				phy_fpy = mat_wt_phy_sum * 100 / mat_wt_sum;
				face_fpy = mat_wt_face_sum * 100 / mat_wt_sum;
				/*压入列值*/
				bcls_ret->Tables[0].Rows.Add();
				bcls_ret->Tables[0].Rows[i]["ITEM_NAME_01"] = factory;
				bcls_ret->Tables[0].Rows[i]["ITEM_NAME_02"] = sg_sign;
				bcls_ret->Tables[0].Rows[i]["ITEM_NAME_03"] = mat_wt_sum.Round(2).ToString();
				bcls_ret->Tables[0].Rows[i]["ITEM_NAME_04"] = fpy.Round(2).ToString();
				bcls_ret->Tables[0].Rows[i]["ITEM_NAME_05"] = judge_fpy.Round(2).ToString();
				bcls_ret->Tables[0].Rows[i]["ITEM_NAME_06"] = phy_fpy.Round(2).ToString();
				bcls_ret->Tables[0].Rows[i]["ITEM_NAME_07"] = face_fpy.Round(2).ToString();
				bcls_ret->Tables[0].Rows[i]["ITEM_NAME_08"] = mat_wt_error_sum.Round(2).ToString();
				i++;
			}
			sel_1.Close();
		}
		Log::Trace("","","查询了[{0}]条记录",i);
		/*.print();*/
		sprintf(s.msg, "查询成功！");

	}

	//捕获数据库操作异常
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = ex.GetMsg();

		//返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;

		//数据库异常时返回-1，事务将被回滚
		doFlag = -1;
	}

	//try块程序发生异常
	catch (const CApplicationException& ex)
	{
		doFlag = ex.GetCode();
		sprintf(s.msg, "%s:%s", (const char*)ex.GetSource(), (const char*)ex.GetMsg());
	}

	//平台异常
	catch (const CException& ex)
	{
		doFlag = -1;
		strcpy(s.msg, (const char*)ex.GetMsg());
	}

	s.flag = doFlag;

	return(doFlag);
}
