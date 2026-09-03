//Dec 16 08:33//请不要修改此行

#include "stdafx.h"

BM2F_ENTERACE(qmtiz1_inq);

int f_qmtiz1_inq(EIClass * bcls_rec,EIClass * bcls_ret,CDbConnection * conn)
{
    //系统日志类定义
	CTracer log(__FUNCTION__);	
	
	//程序用变量
	int doFlag=0;

	CString sqlstr="";

	CString q_mat_no =" ";
	CString q_order_no =" ";
	CString q_pre_check_time_from =" ";
	CString q_pre_check_time_to =" ";
	CString q_pre_check_code =" ";
	CString q_prod_class_code =" ";
	CString q_pono =" ";
	CString q_rep_pono =" ";
	
	try
	{
	    //接收从前台传入的值
		q_mat_no   =bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim();
		q_order_no   =bcls_rec->Tables[0].Rows[0]["ORDER_NO"].ToString().Trim();
		
		q_pre_check_time_from =bcls_rec->Tables[0].Rows[0]["PRE_CHECK_TIME_FROM"].ToString().Trim();
		q_pre_check_time_to   =bcls_rec->Tables[0].Rows[0]["PRE_CHECK_TIME_TO"].ToString().Trim();
		q_pre_check_code      =bcls_rec->Tables[0].Rows[0]["PRE_CHECK_CODE"].ToString().Trim();
		q_prod_class_code     =bcls_rec->Tables[0].Rows[0]["PROD_CLASS_CODE"].ToString().Trim();
		q_pono     = bcls_rec->Tables[0].Rows[0]["PONO"].ToString().Trim();
		q_rep_pono = bcls_rec->Tables[0].Rows[0]["REP_PONO"].ToString().Trim();
		Log::Trace("QMTI","qmtiz1_inq","q_pre_check_time_from[%s],q_pre_check_time_to[%s]",(const char*)q_pre_check_time_from,(const char*)q_pre_check_time_to);
		
		if(q_pre_check_time_from=="")
		{
			q_pre_check_time_from = "00000000";
		}
		if(q_pre_check_time_to=="")
		{
			q_pre_check_time_to = "99999999";
		}
		CString sql_select=" WHERE PRE_CHECK_TIME>='"+q_pre_check_time_from+"000000' "
			               " AND PRE_CHECK_TIME<='"+q_pre_check_time_to+"999999'"
						   ;
		
		if(q_mat_no!="")
		{
			//sql_select+=" AND MAT_NO='"+q_mat_no+"'||'%' ";
			sql_select+=" AND MAT_NO='"+q_mat_no+"' ";
		}
		if(q_order_no!="")
		{
			//sql_select+=" AND MAT_NO='"+q_mat_no+"'||'%' ";
			sql_select+=" AND ORDER_NO='"+q_order_no+"' ";
		}
		if(q_pre_check_code!="")
		{
			//sql_select+=" AND PRE_CHECK_CODE='"+q_pre_check_code+"'||'%' ";
			sql_select+=" AND PRE_CHECK_CODE='"+q_pre_check_code+"' ";
		}
		if(q_prod_class_code!="")
		{
			//sql_select+=" AND PROD_CLASS_CODE='"+q_prod_class_code+"'||'%' ";
			sql_select+=" AND PROD_CLASS_CODE='"+q_prod_class_code+"' ";
		}
		
		if(q_pono!="")
		{
			sql_select+=" AND PONO='"+q_pono+"' ";
		}
		if(q_rep_pono!="")
		{
			sql_select+=" AND REP_PONO='"+q_rep_pono+"' ";
		}

		//获取总条数
		CString sql_count=" SELECT count(1) FROM TQMTIZ1 "+sql_select;
						 
						  
		CDbCommand cmd(sql_count,conn);
		
		CDecimal rc=cmd.ExecuteScalar();
		
		//把值压入RC中，传出前台
		bcls_ret->ExtendedProperties.Add("RC",rc.ToString());
		
		//起始-终止之间
		int nStar=(int)bcls_rec->Tables[1].Rows[0]["START"];
		int nPageSize=(int)bcls_rec->Tables[1].Rows[0]["PAGE_SIZE"];

		//按预检时间倒叙排
		sql_select+=" ORDER BY PRE_CHECK_TIME DESC ";

		sqlstr=" SELECT * FROM TQMTIZ1 "+sql_select;
		
		//Log::Trace("sqlstr{0}",(const char*)sqlstr);		  
		//压入绑定参数，""包括的第一个参数的名称与SQL语句中@后对应的变量完全一致，大小写敏感
		CDbCommand cmd1(sqlstr,conn);
		
	    int count=cmd1.ExecuteQuery(bcls_ret->Tables[0],nStar,nPageSize);
		bcls_ret->Tables[0].set_TableName("QMTIZ1_INQ");
        
        Log::Trace("QMTI","qmtiz1_inq","共查询了{0}条数据",count);
		/*.print();*/
		sprintf(s.msg,"查询成功！");
		
	}
	
	//捕获数据库操作异常
	catch(CDbException& ex)  
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = ex.GetMsg() + "\r\n" + sqlstr;
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}

	//try块程序发生异常
	catch(const CApplicationException& ex)
	{
		doFlag = ex.GetCode();
		sprintf(s.msg,"%s:%s",(const char*)ex.GetSource(),(const char*)ex.GetMsg());
	}

	//平台异常
	catch(const CException& ex)
	{
		doFlag = -1;
		strcpy(s.msg, (const char*)ex.GetMsg());
	}

	s.flag = doFlag;

	return(doFlag);
}