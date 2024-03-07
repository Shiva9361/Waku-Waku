<template>
  <div class="root">
    <h1>Waku-Waku Simulator</h1>
    <div class="content">
      <PipelineHistory :pipeline_history="pipeline_history" />
      <RegisterHistory :register_history="register_history" />
    </div>
    <textarea class="text" rows="10" cols="10"> </textarea>
  </div>
</template>

<script>
import PipelineHistory from "./components/PipelineHistory.vue";
import RegisterHistory from "./components/RegisterHistory.vue";

export default {
  name: "App",
  data() {
    return {
      pipeline_history: [],
      register_history: [],
    };
  },
  components: {
    PipelineHistory,
    RegisterHistory,
  },
  methods: {
    async fetchPipelineHistory() {
      const res = await fetch("http://localhost:5000/core/0/pipe");
      const data = await res.json();
      return data;
    },
    async fetchRegisterHistory() {
      const res = await fetch("http://localhost:5000/core/0/reg");
      const data = await res.json();
      return data;
    },
  },
  async created() {
    this.pipeline_history = await this.fetchPipelineHistory();
    this.register_history = await this.fetchRegisterHistory();
  },
};
</script>

<style scoped>
* {
  box-sizing: border-box;
  margin: 0;
  padding: 0;
}
h1 {
  text-align: center;
}
.text {
  margin-left: 30px;
  width: 500px;
  height: 500px;
  display: inline-block;
}
.content {
  display: inline-block;
}
</style>
