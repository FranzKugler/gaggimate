const fs = require('fs').promises;

function analyzeData(data, tt, tp) {

	const samples = data.samples.filter(s => s.tt === tt && s.tp === tp);
	const volume = samples[samples.length - 1].v - samples[0].v;
	const time = (samples[samples.length - 1].t - samples[0].t) / 1000;
	const pumpVolume = samples.reduce((acc, val) => {
		return {
			lt: val.t,
			pumped: acc.lt === 0 ? 0 : acc.pumped + (val.fl / 1000 * (val.t - acc.lt))
		}
	}, {pumped: 0, lt: 0});
	const actualFlow = volume / time;
	const estimatedFlow = pumpVolume.pumped / time;
	const factor = actualFlow / estimatedFlow;
	return {
		volume,
		time,
		pumpVolume,
		actualFlow,
		estimatedFlow,
		factor
	};

}

async function main() {
	console.log('Starting flow calibration');
	const content = await fs.readFile(process.argv[2], 'utf-8');
	const currentValues = process.argv[3];
	const oneBarFlow = parseFloat(currentValues.split(',')[0]);
	const nineBarFlow = parseFloat(currentValues.split(',')[1]);
	console.log('Current values are 1 bar = ' + oneBarFlow + ', 9 bar = ' + nineBarFlow);
	const data = JSON.parse(content);
	const {
		volume: oneBarVolume,
		time: oneBarTime,
		pumpVolume: oneBarPumpVolume,
		actualFlow: oneBarActualFlow,
		estimatedFlow: oneBarEstimatedFlow,
		factor: oneBarFactor
	}  = analyzeData(data, 25, 1);
	const {
		volume: nineBarVolume,
		time: nineBarTime,
		pumpVolume: nineBarPumpVolume,
		actualFlow: nineBarActualFlow,
		estimatedFlow: nineBarEstimatedFlow,
		factor: nineBarFactor
	}  = analyzeData(data, 25, 9);

	console.log('Volume for 1 bar was ' + oneBarVolume + 'ml in ' + oneBarTime + 's = ' + (oneBarActualFlow) + 'ml/s');
	console.log('Estimated pumped for 1 bar was ' + oneBarPumpVolume.pumped + 'ml in ' + oneBarTime + 's = ' + (oneBarEstimatedFlow) + 'ml/s');
	console.log('Factor for 1 bar flow is ' + oneBarFactor);
	console.log('Volume for 9 bar was ' + nineBarVolume + 'ml in ' + nineBarTime + 's = ' + (nineBarActualFlow) + 'ml/s');
	console.log('Estimated pumped for 9 bar was ' + nineBarPumpVolume.pumped + 'ml in ' + nineBarTime + 's = ' + (nineBarEstimatedFlow) + 'ml/s');
	console.log('Factor for 9 bar flow is ' + nineBarFactor);

	console.log();
	console.log('New pump coefficients: ' + (oneBarFlow * oneBarFactor).toFixed(3) + ',' + (nineBarFlow * nineBarFactor).toFixed(3));
}

main();