/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2024-02-01 15:08:16
Description: 物料改判发送（北）
**************************************************/

#include "stdafx.h"
#include "epex.h"
BM2F_ENTERACE_TELE(cm_002128_rcv)
int f_wmsm_t8p301_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wmsm_t8p302_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_mmsm99(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);//物料修改函数
int f_mmsm_e2t8m1_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wmsm_e2t8m1_miss(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);

int f_cm_002128_rcv(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CModel tqmtsf1("TQMTSF1");
	CModel tmmsm01("TMMSM01");
	CModel tmmsm01_n("TMMSM01");
	CModel tqmts0x("TQMTS0X");
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	EIClass inblock;
	inblock.Tables[0].Columns.Add(tmmsm01);
	inblock.Tables[0].Rows.Clear();
	EIClass inblock1;
	inblock1.Tables[0].Columns.Add(tmmsm01);
	inblock1.Tables[0].Rows.Clear();

	EIClass t8e2m1;
	t8e2m1.Tables[0].set_TableName("E2T8M1");
	t8e2m1.Tables["E2T8M1"].Columns.Add(DT_STRING, "MAT_NO");
	t8e2m1.Tables[0].Rows.Clear();

	EIClass miss_E2T8M1;
	miss_E2T8M1.Tables[0].set_TableName("E2T8M1");
	miss_E2T8M1.Tables["E2T8M1"].Columns.Add(DT_STRING, "MAT_NO");
	miss_E2T8M1.Tables[0].Rows.Clear();
	try
	{
		Log::Trace("", "", "line = {0}", __LINE__);
		//调用物料事件 EICLASS
		EIClass bcls_rec_sm;
		EIClass bcls_ret_sm;
		bcls_rec_sm.Tables[0].set_TableName("MM0099");
		bcls_rec_sm.Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_ID");		/*事件标识*/
		bcls_rec_sm.Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");/*事件产线类型*/
		bcls_rec_sm.Tables["MM0099"].Columns.Add(DT_STRING, "SYSTEM_ID");		/*系统标识*/
		bcls_rec_sm.Tables["MM0099"].Columns.Add(DT_STRING, "FUNC_ID");		/*功能标识*/
		bcls_rec_sm.Tables["MM0099"].Columns.Add(DT_STRING, "FIN_ST_NO");			/*最终出钢记号*/
		bcls_rec_sm.Tables["MM0099"].Columns.Add(DT_STRING, "ST_NO");			/*出钢记号*/
		bcls_rec_sm.Tables["MM0099"].Columns.Add(DT_STRING, "OLD_ST_NO");			/*出钢记号*/
		bcls_rec_sm.Tables["MM0099"].Columns.Add(DT_STRING, "MAT_NO");			/*材料号*/
		bcls_rec_sm.Tables["MM0099"].Columns.Add(DT_STRING, "SLAB_NO");
		bcls_rec_sm.Tables["MM0099"].Columns.Add(DT_STRING, "STEEL_GROUP");
		bcls_rec_sm.Tables["MM0099"].Columns.Add(DT_STRING, "SG_GRADE_1");
		


		tqmtsf1["MSGTYPE"] = bcls_rec->Tables["bapiheader"].Rows[0]["msgtype"].ToString();
		tqmtsf1["FREEUSE1"] = bcls_rec->Tables["bapiheader"].Rows[0]["freeuse1"].ToString();
		tqmtsf1["FREEUSE2"] = bcls_rec->Tables["bapiheader"].Rows[0]["freeuse2"].ToString();
		tqmtsf1["FREEUSE3"] = bcls_rec->Tables["bapiheader"].Rows[0]["freeuse3"].ToString();
		tqmtsf1["FREEUSE4"] = bcls_rec->Tables["bapiheader"].Rows[0]["freeuse4"].ToString();
		tqmtsf1["FREEUSE5"] = bcls_rec->Tables["bapiheader"].Rows[0]["freeuse5"].ToString();

		tqmtsf1["MAT_NO"] = bcls_rec->Tables["t1"].Rows[0]["mat_no"].ToString();
		tqmtsf1.Delete("MAT_NO");

		for (int i = 0; i < bcls_rec->Tables["t1"].Rows.get_Count(); i++)
		{
			tmmsm01["MAT_NO"]= bcls_rec->Tables["t1"].Rows[i]["mat_no"].ToString();
			tmmsm01.Query("MAT_NO");
			tqmtsf1["MAT_NO"] = bcls_rec->Tables["t1"].Rows[i]["mat_no"].ToString();
			tqmtsf1["MATNR_OLD"] = bcls_rec->Tables["t1"].Rows[i]["matnr_old"].ToString();
			tqmtsf1["MATNR_P"] = bcls_rec->Tables["t1"].Rows[i]["matnr_new"].ToString();
			tqmtsf1["CONTRACT_NO"] = bcls_rec->Tables["t1"].Rows[i]["order_id_old"].ToString();
			tqmtsf1["C_SELLID"] = bcls_rec->Tables["t1"].Rows[i]["order_id_new"].ToString();
			tqmtsf1["STGE_LOC"] = bcls_rec->Tables["t1"].Rows[i]["stge_loc"].ToString();
			tqmtsf1["OLD_ST_NO"] = bcls_rec->Tables["t1"].Rows[i]["old_st_no"].ToString();
			tqmtsf1["NEW_ST_NO"] = bcls_rec->Tables["t1"].Rows[i]["new_st_no"].ToString();
			tqmtsf1.TrimOrBlank();
			tqmtsf1.Insert();
			tqmts0x["ST_NO"]= bcls_rec->Tables["t1"].Rows[i]["new_st_no"].ToString();
			tqmts0x.Query("ST_NO");
			CString steel = tqmtsf1["NEW_ST_NO"].ToString().SubstringNE(0, 1) == "1" ? "S" : "C";
			bcls_rec_sm.Tables["MM0099"].Rows.Add();
			//bcls_rec_sm.Tables["MM0099"].Rows[i].Merge(bcls_rec->Tables["t1"].Rows[0]);//获取所有列值
			bcls_rec_sm.Tables["MM0099"].Rows[i]["EVENT_ID"] = "QM73";//物料改判事件
			bcls_rec_sm.Tables["MM0099"].Rows[i]["EVENT_LINE_TYPE"] = "00";
			bcls_rec_sm.Tables["MM0099"].Rows[i]["SYSTEM_ID"] = "QMTS";
			bcls_rec_sm.Tables["MM0099"].Rows[i]["FUNC_ID"] = s.svc_name;
			bcls_rec_sm.Tables["MM0099"].Rows[i]["ST_NO"] = bcls_rec->Tables["t1"].Rows[i]["new_st_no"].ToString();
			bcls_rec_sm.Tables["MM0099"].Rows[i]["FIN_ST_NO"] = bcls_rec->Tables["t1"].Rows[i]["new_st_no"].ToString();
			bcls_rec_sm.Tables["MM0099"].Rows[i]["OLD_ST_NO"] = bcls_rec->Tables["t1"].Rows[i]["old_st_no"].ToString();
			bcls_rec_sm.Tables["MM0099"].Rows[i]["MAT_NO"] = bcls_rec->Tables["t1"].Rows[i]["mat_no"];
			bcls_rec_sm.Tables["MM0099"].Rows[i]["STEEL_GROUP"] = steel + bcls_rec->Tables["t1"].Rows[i]["new_st_no"].ToString();
			bcls_rec_sm.Tables["MM0099"].Rows[i]["SLAB_NO"] = tmmsm01["HEAT_NO"].ToString() + bcls_rec->Tables["t1"].Rows[i]["new_st_no"].ToString() + tmmsm01["SLAB_NO"].ToString().SubstringNE(14);
			bcls_rec_sm.Tables["MM0099"].Rows[i]["SG_GRADE_1"] = tqmts0x["SG_GRADE_1"];
			CString old_if_hr = Db::QueryCString(" select CODE_DESC_3_CONTENT from TWMSMZD02 where CODE_CLASS = 'WM02' and CODE =(select GUIDE_DEST from tmmsm01 where MAT_NO='" + tmmsm01["MAT_NO"].ToString() + "') ");
			if (old_if_hr.Find("1")>=0 && (bcls_rec->Tables["t1"].Rows[i]["old_st_no"].ToString() != bcls_rec->Tables["t1"].Rows[i]["new_st_no"].ToString()))
			{
				
				tmmsm01.MergeTo(inblock.Tables[0], false);
				
				tmmsm01_n["MAT_NO"] = tmmsm01["MAT_NO"];
				tmmsm01_n.Query("MAT_NO");
				tmmsm01_n.MergeTo(inblock1.Tables[0], false);
				
			}
			
		
			tmmsm01.MergeTo(miss_E2T8M1.Tables["E2T8M1"], false);
			
			tmmsm01.MergeTo(t8e2m1.Tables["E2T8M1"], false);
		}
		if (miss_E2T8M1.Tables["E2T8M1"].Rows.get_Count()>0)
		{
			doFlag = f_wmsm_e2t8m1_miss(&miss_E2T8M1, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		

		if (bcls_rec_sm.Tables[0].Rows.get_Count() == 0)
		{
			Log::Trace("", "", "没有需要改判的数据");
			return 0;
		}
		doFlag = f_mmsm99(&bcls_rec_sm, &bcls_ret_sm, conn);

		if (doFlag != 0){
			Log::Trace("", "", "f_mmsm99() msg = [{0}]", s.msg);
			s.flag = -1;
			doFlag = -1;
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (inblock.Tables[0].Rows.get_Count()>0)
		{
			doFlag = f_wmsm_t8p302_snd(&inblock, bcls_ret, conn);
			if (doFlag < 0) {
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}
		if (inblock1.Tables[0].Rows.get_Count() > 0)
		{
			doFlag = f_wmsm_t8p301_snd(&inblock1, bcls_ret, conn);
			if (doFlag < 0) {
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}

		if (t8e2m1.Tables["E2T8M1"].Rows.get_Count() > 0)
		{
			doFlag = f_mmsm_e2t8m1_snd(&t8e2m1, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
	}
	catch (CDbException &ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char *)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException &ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException &ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}
