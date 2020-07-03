package com.yingke.audio.faac.encoder.view;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.RectF;
import android.graphics.SweepGradient;
import android.util.AttributeSet;
import android.view.View;


import com.yingke.audio.faac.encoder.R;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;


/**
 * @author ：chezi008 on 2018/7/23 21:31
 * @description ：
 * @email ：chezi008@qq.com
 */
class RecorderProgressView extends View {
    private static final int DEFAULT_MIN_WIDTH = 400; //View默认最小宽度
    private static final int RED = 230, GREEN = 85, BLUE = 35; //基础颜色，这里是橙红色
    private static final int MIN_ALPHA = 30; //最小不透明度
    private static final int MAX_ALPHA = 255; //最大不透明度
    private static final float doughnutRaduisPercent = 0.65f; //圆环外圆半径占View最大半径的百分比
    private static final float doughnutWidthPercent = 0.12f; //圆环宽度占View最大半径的百分比
    private static final float MIDDLE_WAVE_RADUIS_PERCENT = 0.9f; //第二个圆出现时，第一个圆的半径百分比
    private static final float WAVE_WIDTH = 5f; //波纹圆环宽度

    //圆环颜色
    private static int[] doughnutColors = new int[]{
            Color.argb(MAX_ALPHA, RED, GREEN, BLUE),
            Color.argb(MIN_ALPHA, RED, GREEN, BLUE),
            Color.argb(MIN_ALPHA, RED, GREEN, BLUE)};

    private Paint paint = new Paint(); //画笔
    private float width; //自定义view的宽度
    private float height; //自定义view的高度
    private float currentAngle = 0f; //当前旋转角度
    private float raduis; //自定义view的最大半径
    private float firstWaveRaduis;
    private float secondWaveRaduis;

    private boolean isAnimationStart;
    private ExecutorService esAnimation = Executors.newSingleThreadExecutor();
    private Future ftAnimation;

    public RecorderProgressView(Context context) {
        super(context);
        init();
    }

    public RecorderProgressView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    public RecorderProgressView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init();
    }

    private void init() {

    }

    /**
     * 开始动画
     */
    public void startAnimation() {
        if (!isAnimationStart) {
            isAnimationStart = true;
        }
        if (ftAnimation == null||ftAnimation.isDone()) {
            ftAnimation = esAnimation.submit(animationRunnable);
        }
    }

    public void stopAnimation() {
        isAnimationStart = false;
        ftAnimation.cancel(true);
    }

    private void resetParams() {
        width = getWidth();
        height = getHeight();
        raduis = Math.min(width, height) / 2;
    }

    private void initPaint() {
        paint.reset();
        paint.setAntiAlias(true);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        resetParams();

        //将画布中心设为原点(0,0), 方便后面计算坐标
        canvas.translate(width / 2, height / 2);


        if (currentAngle >= 360f) {
            currentAngle = currentAngle - 360f;
        } else {
            currentAngle = currentAngle + 2f;
        }
        canvas.save();
        //转起来
        canvas.rotate(-currentAngle, 0, 0);
        //画渐变圆环
        float doughnutWidth = raduis * doughnutWidthPercent;//圆环宽度
        //圆环外接矩形
        RectF rectF = new RectF(-raduis * doughnutRaduisPercent, -raduis * doughnutRaduisPercent, raduis * doughnutRaduisPercent, raduis * doughnutRaduisPercent);
        initPaint();
        paint.setStrokeWidth(doughnutWidth);
        paint.setStyle(Paint.Style.STROKE);
        paint.setShader(new SweepGradient(0, 0, doughnutColors, null));
        canvas.drawArc(rectF, 0, 360, false, paint);

        //画旋转头部圆
        initPaint();
        paint.setStyle(Paint.Style.FILL);
        paint.setColor(Color.argb(MAX_ALPHA, RED, GREEN, BLUE));
        canvas.drawCircle(raduis * doughnutRaduisPercent, 0, doughnutWidth / 2, paint);
        canvas.restore();
        //画圆背景
        initPaint();
        paint.setColor(Color.argb(MAX_ALPHA, RED, GREEN, BLUE));
        paint.setAntiAlias(true);
        canvas.drawCircle(0, 0, raduis * doughnutRaduisPercent, paint);
        //画图片
        initPaint();
        Bitmap bitmap = BitmapFactory.decodeResource(getResources(), R.mipmap.ic_mic_white);
        if (bitmap != null) {
            canvas.drawBitmap(bitmap, -bitmap.getWidth() / 2, -bitmap.getHeight() / 2, paint);
        }
        //实现类似水波涟漪效果
        initPaint();
        paint.setStyle(Paint.Style.STROKE);
        paint.setStrokeWidth(WAVE_WIDTH);
        secondWaveRaduis = calculateWaveRaduis(secondWaveRaduis);
        firstWaveRaduis = calculateWaveRaduis(secondWaveRaduis + raduis * (MIDDLE_WAVE_RADUIS_PERCENT - doughnutRaduisPercent) - raduis * doughnutWidthPercent / 2);
        paint.setColor(Color.argb(calculateWaveAlpha(secondWaveRaduis), RED, GREEN, BLUE));
        //画第二个圆（初始半径较小的）
        canvas.drawCircle(0, 0, secondWaveRaduis, paint);

        initPaint();
        paint.setStyle(Paint.Style.STROKE);
        paint.setStrokeWidth(WAVE_WIDTH);
        paint.setColor(Color.argb(calculateWaveAlpha(firstWaveRaduis), RED, GREEN, BLUE));
        //画第一个圆（初始半径较大的）
        canvas.drawCircle(0, 0, firstWaveRaduis, paint);

    }

    /**
     * 计算波纹圆的半径
     *
     * @param waveRaduis
     * @return
     */
    private float calculateWaveRaduis(float waveRaduis) {
        if (waveRaduis < raduis * doughnutRaduisPercent + raduis * doughnutWidthPercent / 2) {
            waveRaduis = raduis * doughnutRaduisPercent + raduis * doughnutWidthPercent / 2;
        }
        if (waveRaduis > raduis * MIDDLE_WAVE_RADUIS_PERCENT + raduis * (MIDDLE_WAVE_RADUIS_PERCENT - doughnutRaduisPercent) - raduis * doughnutWidthPercent / 2) {
            waveRaduis = waveRaduis - (raduis * MIDDLE_WAVE_RADUIS_PERCENT + raduis * (MIDDLE_WAVE_RADUIS_PERCENT - doughnutRaduisPercent) - raduis * doughnutWidthPercent / 2) + raduis * doughnutWidthPercent / 2 + raduis * doughnutRaduisPercent;
        }
        waveRaduis += 0.6f;
        return waveRaduis;
    }

    /**
     * 根据波纹圆的半径计算不透明度
     *
     * @param waveRaduis
     * @return
     */
    private int calculateWaveAlpha(float waveRaduis) {
        float percent = (waveRaduis - raduis * doughnutRaduisPercent - raduis * doughnutWidthPercent / 2) / (raduis - raduis * doughnutRaduisPercent - raduis * doughnutWidthPercent / 2);
        if (percent >= 1f) {
            return 0;
        } else {
            return (int) (MIN_ALPHA * (1f - percent));
        }
    }

    /**
     * 当布局为wrap_content时设置默认长宽
     *
     * @param widthMeasureSpec
     * @param heightMeasureSpec
     */
    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        setMeasuredDimension(measure(widthMeasureSpec), measure(heightMeasureSpec));
    }

    private int measure(int origin) {
        int result = DEFAULT_MIN_WIDTH;
        int specMode = MeasureSpec.getMode(origin);
        int specSize = MeasureSpec.getSize(origin);
        if (specMode == MeasureSpec.EXACTLY) {
            result = specSize;
        } else {
            if (specMode == MeasureSpec.AT_MOST) {
                result = Math.min(result, specSize);
            }
        }
        return result;
    }

    private Runnable animationRunnable = new Runnable() {
        @Override
        public void run() {
            while (isAnimationStart) {
                try {
                    Thread.sleep(10);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                postInvalidate();
            }
        }
    };
}
