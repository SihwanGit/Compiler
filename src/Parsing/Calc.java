package Parsing;
import java.io.*;

public class Calc {
	int token; int value; int ch;
    private PushbackInputStream input;
    final int NUMBER=256;

    Calc(PushbackInputStream is) {
        input = is;
    }
    
    //토큰 읽는 함수 getToken
    int getToken( )  {
        while(true) {
            try  {
	            ch = input.read();
                if (ch == ' ' || ch == '\t' || ch == '\r') ; //공백문자
                else 
                    if (Character.isDigit(ch)) { 
                        value = number( ); //10진수, 256까지
                        input.unread(ch);
                        return NUMBER;
                    }
                    else return ch;
            } catch (IOException e) {
                System.err.println(e);
            }
        }
    }

    //숫자인지 아닌지 판단하고 숫자면 그 숫자를 반환하는 함수
    private int number( )  {
    /* number -> digit { digit } */
        int result = ch - '0';
        try  {
            ch = input.read();
            while (Character.isDigit(ch)) {
                result = 10 * result + ch -'0';
                ch = input.read(); 
            }
        } catch (IOException e) {
            System.err.println(e);
        }
        return result;
    }

    void error( ) {
        System.out.printf("parse error : %d\n", ch);
        //System.exit(1);
    }

    //match 함수, 토큰을 getToken()에게 보냄.
    void match(int c) { 
        if (token == c) 
	    token = getToken();
        else error();
    }

    
    void command( ) {
    /* command -> expr '\n' */
    	//int result = aexp();		// TODO: [Remove this line!!]	
    	Object result = expr();  // TODO: [Use this line for solution]
        if (token == '\n') /* end the parse and print the result */
        	System.out.println("The result is: "+ result); 
        	//이 부분도 The result is: 를 출력하게끔 수정함.
        else error();
    }
    
    
    Object expr() {
    /* <expr> -> <bexp> {& <bexp> | '|'<bexp>} | !<expr> | true | false */
    	Object result;
    	if (token == '!'){ 
    		// !<expr>
    		match('!');
    		result = !(boolean) expr();
    	}
    	else if (token == 't'){
    		// true
    		match('t');
    		result = (boolean)true;
    	}
    	else if (token == 'f'){
    		// false 연산 구현
    		match('f');
    		result = (boolean)false;
    	}
    	else {
    		/* <bexp> {& <bexp> | '|'<bexp>} */
    		result = bexp();
    		while (token == '&' || token == '|') {
    			//비트 연산 구현
    			if (token == '&'){
    				match('&');
    				result = (boolean) result & (boolean) bexp(); //and 연산
    			}
    			else if (token == '|'){
    				match('|');
    				result = (boolean) result & (boolean) bexp(); //or 연산
    			}
    		}
    	}
    	return result;
	}

    
    Object bexp( ) {
    /* <bexp> -> <aexp> [<relop> <aexp>] */
    	Object result;
    	result = ""; // TODO: [Remove this line!!]
    	int aexp1 = aexp(); //앞의 aexp
    	
    	if (token == '<' || token == '>' || token == '=' || token == '!') { // <relop>
    		/* Check each string using relop(): "<", "<=", ">", ">=", "==", "!=" */
    		// TODO: [Fill in your code here]
    		String operator = relop();  // 관계 연산자 가져오기
            int aexp2 = aexp();  // 두 번째 산술 표현식 처리
            
            // 연산자에 따라 비교 수행
            switch (operator) {
                case "<":
                    result = aexp1 < aexp2;
                    break;
                case "<=":
                    result = aexp1 <= aexp2;
                    break;
                case ">":
                    result = aexp1 > aexp2;
                    break;
                case ">=":
                    result = aexp1 >= aexp2;
                    break;
                case "==":
                    result = aexp1 == aexp2;
                    break;
                case "!=":
                    result = aexp1 != aexp2;
                    break;
            }
    	}
		else {
			result = aexp1; //앞에거 반환
		}
    	return result;	//결과 반환
	}

    String relop() {    	
    /* <relop> -> ( < | <= | > | >= | == | != ) */    	
    	String result = "";
    // TODO: [Fill in your code here]
    	//비교 연산자 처리
    	if (token == '<') {
    		match('<');
    	    if (token == '=') {
    	    	match('=');  // '<=' 연산자 처리
    	        result = "<=";
   	        } else {
   	            result = "<";  // '<' 연산자 처리
   	        }
   	    } 
   	    // '>' 관계 연산자 처리
   	    else if (token == '>') {
   	        match('>');
   	        if (token == '=') {
   	            match('=');  // '>=' 연산자 처리
   	            result = ">=";
   	        } else {
   	            result = ">";  // '>' 연산자 처리
   	        }
   	    } 
   	    // '==' 관계 연산자 처리
   	    else if (token == '=') {
   	        match('=');
   	        if (token == '=') {
   	            match('=');  // '==' 연산자 처리
   	            result = "==";
   	        }
   	    } 
   	    // '!=' 관계 연산자 처리
   	    else if (token == '!') {
   	        match('!');
   	        if (token == '=') {
   	            match('=');  // '!=' 연산자 처리
   	            result = "!=";
   	        }
   	    }
    	return result;
	}
	    
    
    // TODO: [Modify code of aexp() for <aexp> -> <term> { + <term> | - <term> }]
    int aexp() {
    /* expr -> term { '+' term } */
        int result = term();
        while(token == '+' || token == '-') {
        	if(token == '+') {
        		match('+');
                result += term();
        	} 
        	else if(token == '-') {
        		match('-');
        		result -= term();
        	}
        }
        return result;
    }

    
    // TODO: [Modify code of term() for <term> -> <factor> { * <factor> | / <factor>}]
    //곱하기 말고 나누기도 가능하게끔 수정해라.
    int term( ) {
    /* term -> factor { '*' factor } */
       int result = factor(); //이게 앞에 factor
       while(token == '*' || token == '/') {
       		if(token == '*') {
       			match('*');
       			result *= term();
       		} 
       		else if(token == '/') {
       			match('/');
       			result /= term();
       		}
       }
       return result;
    }

    
    int factor() {
    /* factor -> '(' expr ')' | number */
    	// TODO: Modify this code to factor -> [-] ( '(' expr ')' | number )
    	
    	boolean isNegativeToken = false; //음/양을 판단하는 토큰 true면 음수다.
        int result = 0;
        if(token == '-') {
        	match('-');
        	isNegativeToken = true;  //앞에서 -가 발견되었으므로 true로 바꾼다.
        } 
        
        if (token == '(') {
            match('(');
            result = aexp();
            match(')');
        }
        else if (token == NUMBER) {
            result = value;
	        match(NUMBER); //token = getToken();
        }
        if(isNegativeToken) result = -result; 
        return result;
    }

    
    void parse( ) {
        token = getToken(); // get the first token
        command();          // call the parsing command
    }

    
    public static void main(String args[]) { 
        Calc calc = new Calc(new PushbackInputStream(System.in));
        while(true) {
            System.out.print(">> ");
            calc.parse();
        	}
        }
}
