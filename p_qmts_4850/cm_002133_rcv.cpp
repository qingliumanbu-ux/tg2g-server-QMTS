/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2024-02-01 15:08:16
Description: PM[炼钢二厂北MES]合同信息变更发送（北）
**************************************************/

#include "stdafx.h"
#include "epex.h"
BM2F_ENTERACE_TELE(cm_002133_rcv)
int f_mmsm99(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);//物料修改函数
int f_cm_002133_rcv(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CModel tqmtsf2("TQMTSF2");
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CModel tmmsm01("TMMSM01");
	try
	{
		//调用物料事件 EICLASS
		EIClass bcls_rec_sm;
		EIClass bcls_ret_sm;
		bcls_rec_sm.Tables[0].set_TableName("MM0099");
		bcls_rec_sm.Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_ID");		/*事件标识*/
		bcls_rec_sm.Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");/*事件产线类型*/
		bcls_rec_sm.Tables["MM0099"].Columns.Add(DT_STRING, "SYSTEM_ID");		/*系统标识*/
		bcls_rec_sm.Tables["MM0099"].Columns.Add(DT_STRING, "FUNC_ID");		/*功能标识*/
		bcls_rec_sm.Tables["MM0099"].Columns.Add(DT_STRING, "ORDER_NO");			/*合同号*/
		bcls_rec_sm.Tables["MM0099"].Columns.Add(DT_STRING, "MAT_NO");			/*材料号*/
		bcls_rec_sm.Tables["MM0099"].Columns.Add(DT_STRING, "SG_SIGN");
		bcls_rec_sm.Tables["MM0099"].Columns.Add(DT_STRING, "WHOLE_BACKLOG");
		bcls_rec_sm.Tables["MM0099"].Columns.Add(DT_STRING, "WHOLE_BACKLOG_NO");
		bcls_rec_sm.Tables["MM0099"].Columns.Add(DT_STRING, "WHOLE_BACKLOG_CODE");
		bcls_rec_sm.Tables["MM0099"].Columns.Add(DT_STRING, "WHOLE_BACKLOG_SEQ");
		bcls_rec_sm.Tables["MM0099"].Columns.Add(DT_STRING, "NEXT_WHOLE_BACKLOG_CODE");
		bcls_rec_sm.Tables["MM0099"].Columns.Add(DT_STRING, "NEXT_WHOLE_BACKLOG_SEQ");
		bcls_rec_sm.Tables["MM0099"].Columns.Add(DT_STRING, "MSC_LINE_NO");
		bcls_rec_sm.Tables["MM0099"].Columns.Add(DT_STRING, "MSC");
		bcls_rec_sm.Tables["MM0099"].Columns.Add(DT_STRING, "APN");
		bcls_rec_sm.Tables["MM0099"].Columns.Add(DT_STRING, "PSC");
		bcls_rec_sm.Tables["MM0099"].Columns.Add(DT_STRING, "PRODUCT_FLAG");
		bcls_rec_sm.Tables["MM0099"].Rows.Clear();

		Log::Trace("", "", "line = {0}", __LINE__);
		tqmtsf2["MSGTYPE"] = bcls_rec->Tables["bapiheader"].Rows[0]["msgtype"].ToString();
		tqmtsf2["FREEUSE1"] = bcls_rec->Tables["bapiheader"].Rows[0]["freeuse1"].ToString();
		tqmtsf2["FREEUSE2"] = bcls_rec->Tables["bapiheader"].Rows[0]["freeuse2"].ToString();
		tqmtsf2["FREEUSE3"] = bcls_rec->Tables["bapiheader"].Rows[0]["freeuse3"].ToString();
		tqmtsf2["FREEUSE4"] = bcls_rec->Tables["bapiheader"].Rows[0]["freeuse4"].ToString();
		tqmtsf2["FREEUSE5"] = bcls_rec->Tables["bapiheader"].Rows[0]["freeuse5"].ToString();

		tqmtsf2["MAT_NO"] = bcls_rec->Tables["zcho_pp_qxbg_rfc"].Rows[0]["mat_no"].ToString();
		tqmtsf2.Delete("MAT_NO");

		for (int i = 0; i < bcls_rec->Tables["zcho_pp_qxbg_rfc"].Rows.get_Count(); i++)
		{
			tqmtsf2["MAT_NO"] = bcls_rec->Tables["zcho_pp_qxbg_rfc"].Rows[i]["mat_no"].ToString();
			tqmtsf2["MARK_POS_CODE"] = bcls_rec->Tables["zcho_pp_qxbg_rfc"].Rows[i]["re_order_flag"].ToString();
			tqmtsf2["MATNR_OLD"] = bcls_rec->Tables["zcho_pp_qxbg_rfc"].Rows[i]["old_material"].ToString();
			tqmtsf2["NEW_PROD_DESC"] = bcls_rec->Tables["zcho_pp_qxbg_rfc"].Rows[i]["new_material"].ToString();
			tqmtsf2["SALESORDERNR"] = bcls_rec->Tables["zcho_pp_qxbg_rfc"].Rows[i]["sell_order_no"].ToString();
			tqmtsf2["SALESORDERLINENR"] = bcls_rec->Tables["zcho_pp_qxbg_rfc"].Rows[i]["sell_order_item_no"].ToString();
			tqmtsf2["WERKS_NEXT"] = bcls_rec->Tables["zcho_pp_qxbg_rfc"].Rows[i]["plant_next"].ToString();
			tqmtsf2["ORDER_NO"] = bcls_rec->Tables["zcho_pp_qxbg_rfc"].Rows[i]["order_no"].ToString();
			tqmtsf2["STEELGRADE"] = bcls_rec->Tables["zcho_pp_qxbg_rfc"].Rows[i]["sg_sign"].ToString();
			tqmtsf2["SLAB_FIN_USE"] = bcls_rec->Tables["zcho_pp_qxbg_rfc"].Rows[i]["slab_fin_use"].ToString();
			tqmtsf2.TrimOrBlank();
			tqmtsf2.Insert();


			bcls_rec_sm.Tables["MM0099"].Rows.Clear();
			bcls_rec_sm.Tables["MM0099"].Rows.Add();
			bcls_rec_sm.Tables["MM0099"].Rows[0].Merge(bcls_rec->Tables["zcho_pp_qxbg_rfc"].Rows[0]);//获取所有列值
			Log::Trace("", "", "zcho_pp_qxbg_rfc = {0}", bcls_rec->Tables["zcho_pp_qxbg_rfc"].Rows.get_Count(), bcls_rec_sm.Tables["MM0099"].Columns.get_Count());
			tmmsm01["MAT_NO"] = tqmtsf2["MAT_NO"];
			tmmsm01.Query("MAT_NO");
			if (bcls_rec->Tables["zcho_pp_qxbg_rfc"].Rows[i]["re_order_flag"].ToString() == "0")
			{
				bcls_rec_sm.Tables["MM0099"].Rows[0]["EVENT_ID"] = "PM01";//脱合同事件
			}
			if (bcls_rec->Tables["zcho_pp_qxbg_rfc"].Rows[i]["re_order_flag"].ToString() == "1"
				|| bcls_rec->Tables["zcho_pp_qxbg_rfc"].Rows[i]["re_order_flag"].ToString() == "2")
			{
				bcls_rec_sm.Tables["MM0099"].Rows[0]["EVENT_ID"] = "PM02";//挂合同事件
			}
			

			if (bcls_rec->Tables["zcho_pp_qxbg_rfc"].Rows[i]["NEXT_WHOLE_BACKLOG_CODE"].ToString() == "9A")
			{
				bcls_rec_sm.Tables["MM0099"].Rows[0]["PRODUCT_FLAG"] = "1";
			}
			else if (bcls_rec->Tables["zcho_pp_qxbg_rfc"].Rows[i]["NEXT_WHOLE_BACKLOG_CODE"].ToString().Trim() == "")
			{
				if (tqmtsf2["NEW_PROD_DESC"].ToString().SubstringNE(0, 1) == "F")
				{
					bcls_rec_sm.Tables["MM0099"].Rows[0]["PRODUCT_FLAG"] = "1";
				}
				else if (tqmtsf2["NEW_PROD_DESC"].ToString().SubstringNE(0, 1) == "H")
				{
					bcls_rec_sm.Tables["MM0099"].Rows[0]["PRODUCT_FLAG"] = "0";
				}
				else
				{
					bcls_rec_sm.Tables["MM0099"].Rows[0]["PRODUCT_FLAG"] = tmmsm01["PRODUCT_FLAG"];
				}
			}
			else
			{
				bcls_rec_sm.Tables["MM0099"].Rows[0]["PRODUCT_FLAG"] = "0";
			}
			
			//re_order_flag 为0时 脱合同；1时在制品挂合同；2时成品挂合同
			bcls_rec_sm.Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "00";
			bcls_rec_sm.Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "QMTS";
			bcls_rec_sm.Tables["MM0099"].Rows[0]["FUNC_ID"] = s.svc_name;
			bcls_rec_sm.Tables["MM0099"].Rows[0]["ORDER_NO"] = bcls_rec->Tables["zcho_pp_qxbg_rfc"].Rows[i]["order_no"].ToString();
			bcls_rec_sm.Tables["MM0099"].Rows[0]["MAT_NO"] = bcls_rec->Tables["zcho_pp_qxbg_rfc"].Rows[i]["mat_no"];
			if (bcls_rec_sm.Tables[0].Rows.get_Count() == 0)
			{
				Log::Trace("", "", "没有需要脱挂的数据");
				return 0;
			}
			doFlag = f_mmsm99(&bcls_rec_sm, &bcls_ret_sm, conn);

			if (doFlag != 0) {
				Log::Trace("", "", "f_mmsm99() msg = [{0}]", s.msg);
				s.flag = -1;
				doFlag = -1;
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
